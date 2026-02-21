#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include "mmu.h"

typedef struct {
    uint16_t PC;
    uint16_t SP;

    uint8_t A;
    uint8_t F;

    uint8_t B;
    uint8_t C;
    uint8_t D;
    uint8_t E;
    uint8_t H;
    uint8_t L;

    MMU *mmu;
} CPU;

void cpu_init(CPU *cpu, MMU *mmu);
int cpu_step(CPU *cpu);

#endif
