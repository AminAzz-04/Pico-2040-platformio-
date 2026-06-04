#include <cstdio>
#include "pico/stdlib.h"
#include "robot.h"
#include "pico/stdlib.h"
#include "pixy2.h"

int main() {
    stdio_init_all();
    sleep_ms(1500);

    pixy2_init();

    Robot robot;  // Oppretter og initialiserer alt.

    robot.calibrateSensors();
    robot.followLine();  // Starter linjefølging (uendelig løkke).

    return 0;
}

