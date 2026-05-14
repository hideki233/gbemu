#include "mmu.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>

void mmu_init(MMU *mmu, const Rom *rom) {
        memset(mmu->memory, 0, sizeof(mmu->memory));
        size_t copiar = (rom->size > 0x8000) ? 0x8000 : rom->size;
        memcpy(&mmu->memory[0x0000], rom->data, copiar);

        /* Estado padrão após boot ROM (aproximação mínima). */
        mmu->memory[0xFF40] = 0x91; /* LCDC: LCD ligado + BG ligado */
        mmu->memory[0xFF44] = 0x00; /* LY */
}

uint8_t mmu_read8(MMU *mmu, uint16_t addr) {

        return mmu->memory[addr];
}

void mmu_write8(MMU *mmu, uint16_t addr, uint8_t value) {
        if (addr < 0x8000) {
                printf("mmu_write: addr=0x%04X\n", addr);
                return;
        }
                mmu->memory[addr] = value;

}
