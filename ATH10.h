//
// Created by Mathias Smidt on 2021/02/24.
//
#include <Wire.h>
#include <Arduino.h>
#include "TempHum.h"
#ifndef TESTBIS_ATH10_H
#define TESTBIS_ATH10_H

class ATH10 {
private:
    void initialize();
    void requestData();
    uint8_t * readData();
    uint32_t getRawTemp(uint8_t * rawData);
    uint32_t getRawHum(uint8_t * rawData);
    float calculateTemp(uint32_t rawTemp);
    float calculateHum(uint32_t rawHum);
    uint8_t readStatus();

public:
    ATH10();
    bool begin(int sda, int scl);
    float getTemp();
    float getHum();
    TempHum getMeasurements();
    void reset();
};
#endif //TESTBIS_ATH10_H
