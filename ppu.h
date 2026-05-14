#ifndef PPU_H
#define PPU_H

#include <stdint.h>

typedef struct {
    uint8_t control;
    uint8_t status;
    uint8_t scrollX;
    uint8_t scrollY;
    uint8_t currentLine;
    // Additional GPU registers and data
} PPU;

#endif
