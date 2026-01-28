//
// Created by azzao on 21.01.2026.
//

#include "qtr.hpp"
#include "config.hpp"
#include "hardware/gpio.h"
#include "pico/time.h"

QtrRcArray::QtrRcArray(const uint* pins, int count) : pins_(pins), count_(count) {}

void QtrRcArray::init() {
    for (int i=0;i<count_;++i) {
        gpio_init(pins_[i]);
        gpio_set_dir(pins_[i], GPIO_OUT);
        gpio_put(pins_[i], 0);
        min_[i] = 65535;
        max_[i] = 0;
    }
}

void QtrRcArray::readRaw(uint16_t* out) {
    // 1) Lad opp (pin = OUTPUT HIGH)
    for (int i=0;i<count_;++i) {
        gpio_set_dir(pins_[i], GPIO_OUT);
        gpio_put(pins_[i], 1);
    }
    sleep_us(10);

    // 2) Slipp (pin = INPUT) og mål tid til den går LOW
    absolute_time_t start = get_absolute_time();
    bool done[32]{};

    for (int i=0;i<count_;++i) { gpio_set_dir(pins_[i], GPIO_IN); done[i]=false; out[i]=QTR_TIMEOUT_US; }

    while (true) {
        uint32_t t = absolute_time_diff_us(start, get_absolute_time());
        if (t >= QTR_TIMEOUT_US) break;

        for (int i=0;i<count_;++i) {
            if (!done[i] && gpio_get(pins_[i]) == 0) {
                out[i] = (uint16_t)t;
                done[i] = true;
            }
        }
        // (valgfritt: tidlig exit hvis alle done)
        bool all=true;
        for (int i=0;i<count_;++i) all &= done[i];
        if (all) break;
    }

    // 3) Tøm
    for (int i=0;i<count_;++i) {
        gpio_set_dir(pins_[i], GPIO_OUT);
        gpio_put(pins_[i], 0);
    }
}

void QtrRcArray::calibrate(int iterations) {
    uint16_t raw[32];
    for (int it=0; it<iterations; ++it) {
        readRaw(raw);
        for (int i=0;i<count_;++i) {
            if (raw[i] < min_[i]) min_[i] = raw[i];
            if (raw[i] > max_[i]) max_[i] = raw[i];
        }
        sleep_ms(5);
    }
}

void QtrRcArray::readCalibrated(uint16_t* out) {
    uint16_t raw[32];
    readRaw(raw);
    for (int i=0;i<count_;++i) {
        uint16_t mn = min_[i], mx = max_[i];
        if (mx <= mn) { out[i]=0; continue; }
        int v = (int)(raw[i] - mn) * 1000 / (int)(mx - mn); // 0..1000
        if (v < 0) v = 0;
        if (v > 1000) v = 1000;
        out[i] = (uint16_t)v;
    }
}

int QtrRcArray::readLine() {
    uint16_t v[32];
    readCalibrated(v);

    // Weighted average, pos 0..(count-1)*1000
    long sum = 0;
    long weighted = 0;
    for (int i=0;i<count_;++i) {
        sum += v[i];
        weighted += (long)v[i] * (long)(i * 1000);
    }
    if (sum == 0) {
        // Ingen linje sett: return midt
        return ((count_-1) * 1000) / 2;
    }
    return (int)(weighted / sum);
}
