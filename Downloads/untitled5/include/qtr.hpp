//
// Created by azzao on 21.01.2026.
//

#ifndef UNTITLED5_QTR_HPP
#define UNTITLED5_QTR_HPP


#pragma once
#include "pico/stdlib.h"

class QtrRcArray {
public:
    QtrRcArray(const uint* pins, int count);

    void init();
    void calibrate(int iterations = 200);
    int  readLine(); // returnerer posisjon 0..(count-1)*1000

private:
    const uint* pins_;
    int count_;
    uint16_t min_[32]{};
    uint16_t max_[32]{};

    void readRaw(uint16_t* out);      // RC tid per sensor
    void readCalibrated(uint16_t* out);
};


#endif //UNTITLED5_QTR_HPP