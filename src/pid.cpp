#include "pid.hpp"
#include <algorithm>

PID::PID(float kp, float ki, float kd,
         float i_min, float i_max, float d_alpha)
    : kp_(kp),
      ki_(ki),
      kd_(kd),
      i_min_(std::min(i_min, i_max)),
      i_max_(std::max(i_min, i_max)),
      d_alpha_(std::clamp(d_alpha, 0.0f, 1.0f)) {}

void PID::reset() {
    i_      = 0.0f;
    prev_   = 0.0f;
    d_filt_ = 0.0f;
    first_  = true;
}

float PID::update(float error, float dt) {
    if (dt <= 0.0f) dt = 0.001f;

    float p_term = kp_ * error;


    float d_term = 0.0f;
    if (!first_) {
        float d_raw = -(error - prev_) / dt;
        d_filt_ += d_alpha_ * (d_raw - d_filt_);
        d_term = kd_ * d_filt_;
    }

    // --- Beregn output uten integral (for anti-windup) ---
    float output_no_i = p_term + d_term;

    // --- Integral med anti-windup ---

    float i_candidate = i_ + ki_ * error * dt;
    i_candidate = std::clamp(i_candidate, i_min_, i_max_);

    float output_candidate = output_no_i + i_candidate;


    bool saturated_high = output_candidate >  1.0f && error > 0.0f;
    bool saturated_low  = output_candidate < -1.0f && error < 0.0f;
    if (!saturated_high && !saturated_low) {
        i_ = i_candidate;
    }

    float output = output_no_i + i_;

    // --- Oppdater tilstand ---
    prev_  = error;
    first_ = false;

    return output;
}
