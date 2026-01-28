//
// Created by azzao on 21.01.2026.
//

#ifndef UNTITLED5_MOTOR_HPP
#define UNTITLED5_MOTOR_HPP
#pragma once
#include "pico/stdlib.h"

class MotorTB6612 {
public:
    MotorTB6612(uint in1, uint in2, uint pwm_pin, bool invert=false);

    void init();
    void set(int speed); // speed: -65535..65535

private:
    uint in1_, in2_, pwm_;
    bool invert_;
    uint slice_;
    uint channel_;
};

#endif //UNTITLED5_MOTOR_HPP