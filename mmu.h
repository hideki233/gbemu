#ifndef MMU_h
#define MMU_h

#include <stdint.h>
#include "rom.h"

typedef struct {
    uint8_t memory[0x10000]; //64kb

}MMU;

void mmu_init(MMU *mmu, const Rom *rom);
uint8_t mmu_read8(MMU *mmu, uint16_t addr);
void mmu_write8(MMU *mmu, uint16_t addr, uint8_t value);

#endif