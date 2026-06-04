
#pragma once
#include <pico/types.h>

// TB6612 motor driver pins
constexpr uint PIN_STBY = 2;

//venstre motor
constexpr uint PIN_AIN1 = 3;
constexpr uint PIN_AIN2 = 4;
constexpr uint PIN_PWMA = 6; // PWM pin for left motor

//høyre motor
constexpr uint PIN_BIN1 = 7;
constexpr uint PIN_BIN2 = 8;
constexpr uint PIN_PWMB = 9; // PWM pin for right motor

// QTR-RC sensor pins
// Adjust these pin numbers according to your wiring
constexpr uint SENSOR_PINS[11] = {10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
constexpr int SENSOR_COUNT = 11;

//control loop parameters
constexpr int BASE_SPEED = 30000; // Base speed for motors
constexpr int MAX_SPEED = 50000;  // Maximum speed for motors

constexpr float KP = 0.025f; // Proportional gain
constexpr float KI = 0.0f;   // Integral gain
constexpr float KD = 0.001f; // Derivative gain
constexpr float PID_I_MIN = -2.0f; // Integral clamp lower bound
constexpr float PID_I_MAX =  2.0f; // Integral clamp upper bound
constexpr float PID_D_ALPHA = 0.2f; // Derivative LPF alpha [0..1]
constexpr float PID_TO_PWM = 12000.0f; // for lite svinging øk fra 1200 til 1600, for mye svinging senk den
constexpr int LOST_TURN_PWM = 14000; // Fixed turn correction when line is lost
constexpr uint32_t LINE_LOST_STOP_MS = 1500; // Stop if line is lost for too long øk hvis den stopper for fort
constexpr uint32_t FOLLOW_LOOP_TIMEOUT_MS = 180000; // Hard stop timeout for safety

constexpr uint32_t SENSOR_TIMEOUT_US = 2500; // Timeout for QTR-RC sensor reading

#ifndef QTR_TIMEOUT_US
#define QTR_TIMEOUT_US 3000u
#endif
