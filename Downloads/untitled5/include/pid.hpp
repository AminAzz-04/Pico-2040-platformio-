//
// Created by azzao on 21.01.2026.
//

#ifndef UNTITLED5_PID_HPP
#define UNTITLED5_PID_HPP


#pragma once

class PID {
public:
    PID(float kp, float ki, float kd);
    float update(float error, float dt);
    void reset();

private:
    float kp_, ki_, kd_;
    float i_ = 0.0f;
    float prev_ = 0.0f;
    bool first_ = true;
};



#endif //UNTITLED5_PID_HPP