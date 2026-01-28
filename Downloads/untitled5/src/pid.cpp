//
// Created by azzao on 21.01.2026.
//

#include "pid.hpp"

PID::PID(float kp, float ki, float kd) : kp_(kp), ki_(ki), kd_(kd) {}

void PID::reset() { i_=0; prev_=0; first_=true; }

float PID::update(float error, float dt) {
    if (dt <= 0) dt = 1e-3f;

    i_ += error * dt;

    float d = 0.0f;
    if (!first_) d = (error - prev_) / dt;
    first_ = false;
    prev_ = error;

    return kp_*error + ki_*i_ + kd_*d;
}
