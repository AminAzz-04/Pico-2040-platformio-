// PID controller header.
// Created by azzao on 21.01.2026.

#ifndef pid_hpp
#define pid_hpp

#pragma once

class PID {
public:
    PID(float kp, float ki, float kd,
        float i_min = -1.0f, float i_max = 1.0f,
        float d_alpha = 0.2f);

    float update(float error, float dt);
    void reset();

private:
    const float kp_;
    const float ki_;
    const float kd_;
    const float i_min_;
    const float i_max_;
    const float d_alpha_;
    float i_       = 0.0f;
    float prev_    = 0.0f;
    float d_filt_  = 0.0f;
    bool  first_   = true;
};

#endif // pid_hpp