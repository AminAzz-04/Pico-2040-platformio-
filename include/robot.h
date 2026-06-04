// Created by Amin on 27/02/2026.

#ifndef PICO_2040_PLATFORMIO_ROBOT_H
#define PICO_2040_PLATFORMIO_ROBOT_H

#include "motor_tb6612.hpp"
#include "pid.hpp"
#include "config.hpp"
#include "pixy2.h"

class Robot {
public:
    Robot();
    void calibrateSensors();
    void followLine();
    void stop();
private:
    MotorTB6612 left;
    MotorTB6612 right;
    PID pid;
    Pixy2Line pixyLine;
};

#endif // PICO_2040_PLATFORMIO_ROBOT_H

