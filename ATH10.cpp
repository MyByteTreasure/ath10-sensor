//
// Created by Mathias Smidt on 2021/02/25.
//
#include "ATH10.h"

/*
 * This information is retrieved from the ATH10 sensor datasheet.
 * There are three commands in total: initialize (INIT_CMD), request measurement (MEAS_CMD), reset (RESET_CMD).
 * The answer size is the amount of bytes that is send back by sensor after a measurement request
 * The Address is used for the I2C communication
 */
const static uint8_t INIT_CMD[3] = {0xE1, 0x08, 0x00};
const static uint8_t MEAS_CMD[3] = {0xAC, 0x33, 0x00};
const static uint8_t RESET_CMD = 0xBA;
const static uint8_t ANSWER_SIZE = 6;
const static uint8_t ADDRESS = 0x38;;

/*
 * Send initialize command to sensor over I2C
 */
void ATH10::initialize() {
    Wire.beginTransmission(ADDRESS);
    Wire.write(INIT_CMD, sizeof(INIT_CMD));
    Wire.endTransmission();
}

/*
 * Send measurement request over I2C note that a delay between requesting data and reading the result is needed as
 * the sensor needs time to perform measurement.
 */
void ATH10::requestData() {
    Wire.beginTransmission(ADDRESS);
    Wire.write(MEAS_CMD, sizeof(MEAS_CMD));
    Wire.endTransmission();
}
/*
 * After measurement request has been sent, the sensor will send the data over I2C.
 * The sensor will send 6 bytes:
 * Byte 1 is status
 * Byte 2, 3 and the 4 lef bits of byte 4 are humidity data
 * Right 4 bits of byte 4, byte 5 and 6 are temp data
 */
uint8_t *ATH10::readData() {
    Wire.requestFrom(ADDRESS, ANSWER_SIZE);
    static uint8_t data[ANSWER_SIZE];
    for (int i = 0; Wire.available() > 0 ; ++i) {
        data[i] = Wire.read();
        /*
        Serial.print("0x");
        Serial.print(* (data + i), HEX);
        Serial.print("\t");
         */
    }
    return data;
}

/*
 * Extract the raw temperature int number for the rawdata bytes send by the sensor
 */
uint32_t ATH10::getRawTemp(uint8_t * rawData) {
    uint32_t tempBytes = ((rawData[3] & 0x0F) << 16) | (rawData[4] << 8) | rawData[5];
    return tempBytes;
}

/*
 * Extract the raw humidity int number for the rawdata bytes send by the sensor
 */
uint32_t ATH10::getRawHum(uint8_t *rawData) {
    uint32_t hmdBytes = ((rawData[1] << 16) | (rawData[2] << 8) | rawData[3]) >> 4;
    return hmdBytes;
}

float ATH10::calculateTemp(uint32_t rawTemp) {
    return ((float) rawTemp * 200.0 / 1048576.0) - 50.0;
}

float ATH10::calculateHum(uint32_t rawHum) {
    return ((float) rawHum / 1048576.0) * 100.0;
}

uint8_t ATH10::readStatus() {
    uint8_t result = 0x00;
    uint8_t answer_size = 1;
    Wire.requestFrom(ADDRESS, answer_size);
    result = Wire.read();
    return result;
}

ATH10::ATH10() {
}

//Initializes I2C connection
bool ATH10::begin(int sda, int scl) {
    Wire.begin(sda, scl);
    initialize();
    delay(250);
    if ((readStatus() & 0x60) == 0x00) {
        return true;
    } else {
        return false;
    }
}


float ATH10::getTemp() {
    requestData();
    delay(150);
    uint8_t * data = readData();
    uint32_t rawTemp = getRawTemp(data);
    float temperature = calculateTemp(rawTemp);
    return temperature;
}

float ATH10::getHum() {
    requestData();
    delay(150);
    uint8_t * data = readData();
    uint32_t rawHum = getRawHum(data);
    float humidity = calculateHum(rawHum);
    return humidity;
}

TempHum ATH10::getMeasurements() {
    requestData();
    delay(150);
    uint8_t * data = readData();
    uint32_t rawHum = getRawHum(data);
    uint32_t rawTemp = getRawTemp(data);
    TempHum tempHum{};
    tempHum.temperature = calculateTemp(rawTemp);
    tempHum.humidity = calculateHum(rawHum);
    return tempHum;
}

void ATH10::reset() {
    Wire.beginTransmission(ADDRESS);
    Wire.write(RESET_CMD);
    Wire.endTransmission();
}
