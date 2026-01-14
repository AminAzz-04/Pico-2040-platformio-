#include "tb6612.hpp"
#include "pico/stdlib.h"
#include "hardware/pwm.h"

static uint32_t compute_wrap(float pwm_hz) {
    // PWM clock = 125 MHz on RP2040 by default
    // Use divider=1.0 and compute wrap
    const float clk = 125000000.0f;
    uint32_t wrap = (uint32_t)(clk / pwm_hz) - 1u;
    if (wrap > 65535u) wrap = 65535u;
    if (wrap < 1000u)  wrap = 1000u;
    return wrap;
}

TB6612::TB6612(uint stby,
               uint ain1, uint ain2, uint pwma,
               uint bin1, uint bin2, uint pwmb,
               float pwm_hz)
: stby_(stby), ain1_(ain1), ain2_(ain2), pwma_(pwma),
  bin1_(bin1), bin2_(bin2), pwmb_(pwmb) {

    gpio_init(stby_); gpio_set_dir(stby_, GPIO_OUT); gpio_put(stby_, 0);

    gpio_init(ain1_); gpio_set_dir(ain1_, GPIO_OUT);
    gpio_init(ain2_); gpio_set_dir(ain2_, GPIO_OUT);
    gpio_init(bin1_); gpio_set_dir(bin1_, GPIO_OUT);
    gpio_init(bin2_); gpio_set_dir(bin2_, GPIO_OUT);

    // PWM pins
    gpio_set_function(pwma_, GPIO_FUNC_PWM);
    gpio_set_function(pwmb_, GPIO_FUNC_PWM);

    slice_a_ = pwm_gpio_to_slice_num(pwma_);
    slice_b_ = pwm_gpio_to_slice_num(pwmb_);

    wrap_ = compute_wrap(pwm_hz);

    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_clkdiv(&cfg, 1.0f);
    pwm_config_set_wrap(&cfg, (uint16_t)wrap_);

    pwm_init(slice_a_, &cfg, true);
    pwm_init(slice_b_, &cfg, true);

    pwm_set_gpio_level(pwma_, 0);
    pwm_set_gpio_level(pwmb_, 0);
}

void TB6612::enable(bool on) {
    gpio_put(stby_, on ? 1 : 0);
}

void TB6612::set_motor(uint in1, uint in2, uint pwm_pin, uint /*slice*/, int pwm) {
    if (pwm == 0) {
        // coast
        gpio_put(in1, 0); gpio_put(in2, 0);
        pwm_set_gpio_level(pwm_pin, 0);
        return;
    }

    bool forward = pwm > 0;
    int mag = forward ? pwm : -pwm;
    if (mag > (int)wrap_) mag = (int)wrap_;

    gpio_put(in1, forward ? 1 : 0);
    gpio_put(in2, forward ? 0 : 1);
    pwm_set_gpio_level(pwm_pin, (uint16_t)mag);
}

void TB6612::set_left(int pwm)  { set_motor(ain1_, ain2_, pwma_, slice_a_, pwm); }
void TB6612::set_right(int pwm) { set_motor(bin1_, bin2_, pwmb_, slice_b_, pwm); }

void TB6612::stop() {
    set_left(0);
    set_right(0);
}
