// Robot implementation.
// Created by Amin on 27/02/2026.

#include "robot.h"
#include <pico/stdlib.h>
#include <cstdio>
#include <algorithm>

Robot::Robot()
    : left(PIN_AIN1, PIN_AIN2, PIN_PWMA, false),
      right(PIN_BIN1, PIN_BIN2, PIN_PWMB, true),
      pid(KP, KI, KD, PID_I_MIN, PID_I_MAX, PID_D_ALPHA) {

    gpio_init(PIN_STBY);
    gpio_set_dir(PIN_STBY, GPIO_OUT);
    gpio_put(PIN_STBY, 1);

    left.init();
    right.init();
}

void Robot::calibrateSensors() {
    printf("Ingen kalibrering nødvendig med Pixy2.\n");
}

void Robot::followLine() {
    pid.reset();

    absolute_time_t started       = get_absolute_time();
    absolute_time_t last          = get_absolute_time();
    absolute_time_t last_line_seen = last;
    int dbg = 0;

    while (true) {
        absolute_time_t now = get_absolute_time();

        uint64_t runtime_us = absolute_time_diff_us(started, now);
        if (runtime_us >= (uint64_t)FOLLOW_LOOP_TIMEOUT_MS * 1000ULL) {
            printf("Safety stop: loop timeout (%lu ms)\n", FOLLOW_LOOP_TIMEOUT_MS);
            stop();
            break;
        }

        float dt = absolute_time_diff_us(last, now) / 1e6f;
        last = now;
        if (dt < 0.001f) dt = 0.001f;
        if (dt > 0.05f)  dt = 0.05f;

        int result = pixy2_getMainFeatures(&pixyLine);
        bool line_seen = (result > 0 && pixyLine.numVectors > 0);

        float error = 0.0f;
        if (line_seen) {
            error = (39.0f - (float)pixyLine.vectors[0].x1) / 39.0f;
            last_line_seen = now;
        }

        float corr_pwm = 0.0f;
        if (line_seen) {
            corr_pwm = pid.update(error, dt) * PID_TO_PWM;
        } else {
            pid.reset();
            corr_pwm = (error >= 0.0f)
                       ? (float)LOST_TURN_PWM
                       : (float)-LOST_TURN_PWM;

            uint64_t lost_us = absolute_time_diff_us(last_line_seen, now);
            if (lost_us >= (uint64_t)LINE_LOST_STOP_MS * 1000ULL) {
                printf("Safety stop: line lost (%lu ms)\n", LINE_LOST_STOP_MS);
                stop();
                break;
            }
        }

        int l = BASE_SPEED + (int)corr_pwm;
        int r = BASE_SPEED - (int)corr_pwm;

        l = std::max(-MAX_SPEED, std::min(MAX_SPEED, l));
        r = std::max(-MAX_SPEED, std::min(MAX_SPEED, r));

        left.set(l);
        right.set(r);

        if (++dbg >= 50) {
            dbg = 0;
            printf("seen=%d err=%.2f corr=%.1f L=%d R=%d dt=%.3f\n",
                   line_seen ? 1 : 0, error, corr_pwm, l, r, dt);
        }

        sleep_ms(2);
    }
}

void Robot::stop() {
    left.set(0);
    right.set(0);
}