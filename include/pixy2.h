//
// Created by Amin on 14/04/2026.
//

#ifndef PICO_2040_PLATFORMIO_PIXY2_H
#define PICO_2040_PLATFORMIO_PIXY2_H
#pragma once
#include "pico/stdlib.h"

// Pindefinisjoner
#define PIXY_SPI      spi0
#define PIXY_MISO     16
#define PIXY_SCK      18
#define PIXY_MOSI     19
#define PIXY_SS       17
#define PIXY_BAUDRATE 2000000

// Protokollkonstantar
#define PIXY_SYNC_NOCS   0xc1ae
#define PIXY_SYNC_CS     0xc1af
#define PIXY_REQ_LINE    48

// Linjefeature-flagg
#define LINE_VECTOR      1
#define LINE_INTERSECTION 2
#define LINE_BARCODE     4

struct Vector {
    uint8_t x0, y0;  // hale (nærast robot)
    uint8_t x1, y1;  // hovud (lengst frå robot)
    uint8_t index;
    uint8_t flags;
};

struct Pixy2Line {
    uint8_t  numVectors;
    Vector   vectors[10];
};

void     pixy2_init();
int      pixy2_getMainFeatures(Pixy2Line *result);




#endif //PICO_2040_PLATFORMIO_PIXY2_H