#include <cstdio>
#include "pico/stdlib.h"

#include "config.hpp"
#include "motor_tb6612.hpp"
#include "qtr.hpp"
#include "pid.hpp"

static inline int clamp_i(int v, int lo, int hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

int main() {
    stdio_init_all();
    sleep_ms(1500);

    // STBY
    gpio_init(PIN_STBY);
    gpio_set_dir(PIN_STBY, GPIO_OUT);
    gpio_put(PIN_STBY, 1);

    MotorTB6612 left(PIN_AIN1, PIN_AIN2, PIN_PWMA, false);
    MotorTB6612 right(PIN_BIN1, PIN_BIN2, PIN_PWMB, true); // ofte må én inverteres
    left.init();
    right.init();

    QtrRcArray qtr(SENSOR_PINS, SENSOR_COUNT);
    qtr.init();

    PID pid(KP, KI, KD);

    printf("Calibrating... move over line!\n");
    qtr.calibrate(250);
    printf("Done.\n");

    absolute_time_t last = get_absolute_time();

    const int center = ((SENSOR_COUNT - 1) * 1000) / 2;

    while (true) {
        int pos = qtr.readLine();            // 0..(N-1)*1000
        int error = center - pos;            // sign bestemmer retning

        absolute_time_t now = get_absolute_time();
        float dt = absolute_time_diff_us(last, now) / 1e6f;
        last = now;

        float corr = pid.update((float)error, dt);

        int l = (int)BASE_SPEED + (int)corr;
        int r = (int)BASE_SPEED - (int)corr;

        l = clamp_i(l, -MAX_SPEED, MAX_SPEED);
        r = clamp_i(r, -MAX_SPEED, MAX_SPEED);

        left.set(l);
        right.set(r);

        // Debug
        printf("pos=%d err=%d corr=%.1f L=%d R=%d\n", pos, error, corr, l, r);
        sleep_ms(10);
    }
}
