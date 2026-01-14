#include <cstdio>
#include "pico/stdlib.h"
#include "tb6612.hpp"
#include "qtr_rc.hpp"
#include "pid.hpp"

static const int SENSOR_N = 11;
static const uint8_t SENSOR_PINS[SENSOR_N] = {2,3,4,5,6,7,8,9,10,11,12}; // bytt til deres pins

int main() {
    stdio_init_all();
    sleep_ms(1200);

    // TB6612 pins (EKSEMPEL) - bytt til deres wiring
    TB6612 drv(
        /*stby*/ 15,
        /*ain1*/ 16, /*ain2*/ 17, /*pwma*/ 18,
        /*bin1*/ 19, /*bin2*/ 20, /*pwmb*/ 21,
        20000.0f
    );
    drv.enable(true);

    QtrRc qtr(SENSOR_PINS, SENSOR_N);

    PID pid;
    pid.kp = 0.02f; // startverdi (må tunes)
    pid.ki = 0.0f;
    pid.kd = 0.001f;

    uint16_t s[SENSOR_N];

    const float control_hz = 500.0f;
    const float dt = 1.0f / control_hz;
    const int base_pwm = 9000; // avhenger av wrap_; juster senere

    absolute_time_t next = get_absolute_time();

    while (true) {
        // fast loop
        next = delayed_by_us(next, (int)(1e6f / control_hz));
        sleep_until(next);

        qtr.read_raw(s, 2500);

        // TODO: kalibrering + normalisering
        // TODO: posisjon/feil

        // midlertidig: print sensor
        static int decim = 0;
        if (++decim >= 25) { // 500/25=20 Hz print
            decim = 0;
            for (int i = 0; i < SENSOR_N; ++i) {
                printf("%u\t", s[i]);
            }
            printf("\n");
        }

        // midlertidig: motor-test (kommenter ut når sensor virker)
        drv.set_left(base_pwm);
        drv.set_right(base_pwm);
    }
}
