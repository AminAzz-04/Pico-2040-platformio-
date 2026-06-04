//
// Created by azzao on 21.01.2026.
//
#include "motor_tb6612.hpp"
#include "hardware/pwm.h"
#include "hardware/gpio.h"

static inline uint16_t clamp_u16(int v) {
    if (v < 0) return 0;
    if (v > 65535) return 65535;
    return (uint16_t)v;
}

MotorTB6612::MotorTB6612(uint in1, uint in2, uint pwm_pin, bool invert)
: in1_(in1), in2_(in2), pwm_(pwm_pin), invert_(invert) {}

void MotorTB6612::init() {
    gpio_init(in1_); gpio_set_dir(in1_, GPIO_OUT);
    gpio_init(in2_); gpio_set_dir(in2_, GPIO_OUT);

    gpio_set_function(pwm_, GPIO_FUNC_PWM);
    slice_ = pwm_gpio_to_slice_num(pwm_);
    channel_ = pwm_gpio_to_channel(pwm_);

    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_wrap(&cfg, 65535); // 16-bit
    pwm_init(slice_, &cfg, true);

    pwm_set_chan_level(slice_, channel_, 0);
    gpio_put(in1_, 0);
    gpio_put(in2_, 0);
}

void MotorTB6612::set(int speed) {
    if (invert_) speed = -speed;

    bool forward = speed >= 0;
    int mag = forward ? speed : -speed;
    uint16_t duty = clamp_u16(mag);

    // Direction
    gpio_put(in1_, forward ? 1 : 0);
    gpio_put(in2_, forward ? 0 : 1);

    // PWM
    pwm_set_chan_level(slice_, channel_, duty);
}

