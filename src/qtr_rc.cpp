#include "qtr_rc.hpp"
#include "pico/stdlib.h"
#include "hardware/timer.h"

QtrRc::QtrRc(const uint8_t* pins, int count) : pins_(pins), count_(count) {
    for (int i = 0; i < count_; ++i) {
        gpio_init(pins_[i]);
        gpio_set_dir(pins_[i], GPIO_IN);
        gpio_pull_down(pins_[i]); // optional
    }
}

void QtrRc::read_raw(uint16_t* out, uint32_t timeout_us) {
    // 1) charge caps: set output high briefly
    for (int i = 0; i < count_; ++i) {
        gpio_set_dir(pins_[i], GPIO_OUT);
        gpio_put(pins_[i], 1);
    }
    sleep_us(10);

    // 2) switch to input and time until they go low
    for (int i = 0; i < count_; ++i) {
        gpio_set_dir(pins_[i], GPIO_IN);
    }

    absolute_time_t start = get_absolute_time();
    uint32_t t;
    bool done[32] = {false};

    for (int i = 0; i < count_; ++i) out[i] = (uint16_t)timeout_us;

    while (true) {
        t = (uint32_t)absolute_time_diff_us(start, get_absolute_time());
        if (t >= timeout_us) break;

        for (int i = 0; i < count_; ++i) {
            if (!done[i] && gpio_get(pins_[i]) == 0) {
                out[i] = (uint16_t)t;
                done[i] = true;
            }
        }
    }
}
