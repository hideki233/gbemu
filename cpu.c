#include "cpu.h"

#include <stdio.h>



void cpu_init(CPU *cpu, MMU *mmu) {
    cpu->mmu = mmu;

    //Aqui vai skipar o boot(provisoriamente), pulando diretamente para o 0x0100 onde comeca o jogo em si
    cpu->PC = 0x0100;
    cpu->SP = 0xFFFE;

    cpu->A = cpu->F = 0;
    cpu->B = cpu->C = 0;
    cpu->D = cpu->E = 0;
    cpu->H = cpu->L = 0;

}

int cpu_step(CPU *cpu) {
    uint8_t opcode = mmu_read8(cpu->mmu, cpu->PC);
    cpu->PC++;

    switch (opcode) {
        case 0x00: {
            return 4;
        }
        case 0x3E:{
            uint8_t n = mmu_read8(cpu->mmu, cpu->PC);
            cpu->PC++;
            cpu->A = n;
            return 8;
        }
        case 0xC3:{
            uint8_t low = mmu_read8(cpu->mmu, cpu->PC);
            cpu->PC++;
            uint8_t high = mmu_read8(cpu->mmu, cpu->PC);
            cpu->PC++;
            uint16_t addr = ((uint16_t)high << 8) | low;
            cpu->PC = addr;
            return 16;
        }
        case 0xFE: {
            uint8_t n = mmu_read8(cpu->mmu, cpu->PC);
            cpu->PC++;

            uint8_t A = cpu->A;

            cpu->F = 0x00;//Zera as flags
            cpu->F |= 0x40;//Aciona flag de Subtracao

            if (A == n) cpu->F |= 0x80; //Aciona a flag Z caso valor igual a zero
            if (A < n) cpu->F |= 0x10; //Aciona a flag C caso valor negativo
            return 8;
        }
        case 0x28: {
            uint8_t off = (uint8_t)mmu_read8(cpu->mmu, cpu->PC);
            cpu->PC++;

            if (cpu->F & 0x80) {
                cpu->PC = (uint16_t)(cpu->PC + off);
                return 12;
            }
            return 8;
        }
        case 0xAF: {
            cpu->A = 0;
            cpu->F = 0x80;
            return 4;
        }
        case 0x18: {
            uint8_t off = (uint8_t)mmu_read8(cpu->mmu, cpu->PC);
            cpu->PC++;
            cpu->PC = (uint16_t)(cpu->PC + off);
            return 12;
        }
        case 0xEA: {
            uint8_t low = mmu_read8(cpu->mmu, cpu->PC);
            cpu->PC++;
            uint8_t high = mmu_read8(cpu->mmu, cpu->PC);
            cpu->PC++;

            uint16_t addr = ((uint16_t)high << 8) | low;
            mmu_write8(cpu->mmu, addr, cpu->A);
            printf("Escreveu A=0x%02X em addr=0x%04X\n", cpu->A, addr);
            return 16;
        }
        default: {
            for (int i = 0; i < 5; i++) {
                printf("OPCODE nao implementado: 0x%02X (PC=0x%04X)\n", opcode, (uint16_t)(cpu->PC - 1));
                cpu->PC++;
            }
            return 0;
        }
    }
}