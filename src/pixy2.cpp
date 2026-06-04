//
// Created by Amin on 14/04/2026.
//

#include "../include/pixy2.h"
#include "pixy2.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"

static void ss_low()  { gpio_put(PIXY_SS, 0); sleep_us(10); }
static void ss_high() { gpio_put(PIXY_SS, 1); sleep_us(10); }

static void spi_send(uint8_t *data, uint8_t len) {
    spi_write_blocking(PIXY_SPI, data, len);
}

static void spi_recv(uint8_t *data, uint8_t len) {
    spi_read_blocking(PIXY_SPI, 0, data, len);
}

void pixy2_init() {
    spi_init(PIXY_SPI, PIXY_BAUDRATE);
    spi_set_format(PIXY_SPI, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    gpio_set_function(PIXY_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIXY_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIXY_MOSI, GPIO_FUNC_SPI);

    gpio_init(PIXY_SS);
    gpio_set_dir(PIXY_SS, GPIO_OUT);
    gpio_put(PIXY_SS, 1);

    sleep_ms(500); // vent på Pixy2 oppstart
}

int pixy2_getMainFeatures(Pixy2Line *result) {
    result->numVectors = 0;

    // Send request
    // Format: sync(2) + type(1) + len(1) + features(1) + wait(1)
    uint8_t req[] = {
        0xae, 0xc1,  // sync bytes (little endian)
        0x30,        // type = getMainFeatures
        0x02,        // payload length = 2
        0x07,        // features = vector|intersection|barcode
        0x01         // wait = true
    };

    ss_low();
    spi_send(req, sizeof(req));
    ss_high();

    sleep_us(500);

    // Les header: sync(2) + type(1) + len(1) + checksum(2) = 6 byte
    ss_low();
    uint8_t header[6];
    spi_recv(header, 6);

    // Sjekk sync
    uint16_t sync = header[0] | (header[1] << 8);
    if (sync != PIXY_SYNC_CS) {
        ss_high();
        return -1;
    }

    uint8_t payloadLen = header[3];
    if (payloadLen == 0) {
        ss_high();
        return 0;
    }

    uint8_t payload[128];
    if (payloadLen > sizeof(payload)) {
        ss_high();
        return -1;
    }
    spi_recv(payload, payloadLen);
    ss_high();

    int i = 0;
    while (i < payloadLen) {
        uint8_t featureType = payload[i++];
        uint8_t featureLen  = payload[i++];

        if (featureType == LINE_VECTOR) {
            int numVecs = featureLen / 6; // kvar vector = 6 byte
            for (int v = 0; v < numVecs && result->numVectors < 10; v++) {
                Vector *vec = &result->vectors[result->numVectors++];
                vec->x0    = payload[i++];
                vec->y0    = payload[i++];
                vec->x1    = payload[i++];
                vec->y1    = payload[i++];
                vec->index = payload[i++];
                vec->flags = payload[i++];
            }
        } else {
            i += featureLen;
        }
    }

    return result->numVectors;
}
