#ifndef PICO_PROJECT_PID_HPP
#define PICO_PROJECT_PID_HPP
#pragma once
#include <cstdint>

struct PID {
    float kp{0}, ki{0}, kd{0};
    float integral{0};
    float last_error{0};
    float i_limit{2000.0f};   // clamp integral

    void reset() { integral = 0; last_error = 0; }

    float step(float error, float dt) {
        integral += error * dt;
        if (integral > i_limit) integral = i_limit;
        if (integral < -i_limit) integral = -i_limit;

        float deriv = (error - last_error) / dt;
        last_error = error;
        return kp*error + ki*integral + kd*deriv;
    }
};


#endif //PICO_PROJECT_PID_HPP