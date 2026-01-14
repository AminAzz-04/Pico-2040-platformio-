#ifndef PICO_PROJECT_TB6612_HPP
#define PICO_PROJECT_TB6612_HPP

#pragma once
#include <cstdint>

class tb6612 {
public:
    tb6612(uint stby,
           uint ain1, uint ain2, uint pwma,
           uint bin1, uint bin2, uint pwmb,
           float pwm_hz = 20000.0f);

    void enable(bool on);
    void set_left(int pwm);
    void set_right(int pwm);
    void stop();

    int max_pwm() const { return (int)wrap_;}

private:
    void set_motor(uint in1, uint in2, uint pwm_pin, uint slice, int pwm);

    uint stby_, ain1_, aint2_, pwma_, bin1_, bin2_, pwmb_;
    uint slice_a_, slice_b_;
    uint32_t wrap_;
};



#endif //PICO_PROJECT_TB6612_HPP