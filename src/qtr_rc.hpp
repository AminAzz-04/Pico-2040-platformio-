#ifndef PICO_PROJECT_QTR_RC_HPP
#define PICO_PROJECT_QTR_RC_HPP


#pragma once
#include <cstdint>

class QtrRc {
public:
    QtrRc(const uint8_t* pins, int count);

    // les rå RC-tider (0..timeout_us)
    void read_raw(uint16_t* out, uint32_t timeout_us = 2500);

    int count() const { return count_; }

private:
    const uint8_t* pins_;
    int count_;
};



#endif //PICO_PROJECT_QTR_RC_HPP