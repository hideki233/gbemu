#include "cpu.h"

#include <stdio.h>



void cpu_init(CPU *cpu, MMU *mmu) {
    cpu->mmu = mmu;

    //Aqui vai skipar o boot(provisoriamente), pulando diretamente para o 0x0100 onde comeca o jogo em si
    cpu->PC = 0x0100;
    cpu->SP = 0xFFFE;
    cpu->ime = 0;

    cpu->A = cpu->F = 0;
    cpu->B = cpu->C = 0;
    cpu->D = cpu->E = 0;
    cpu->H = cpu->L = 0;


}
static uint16_t get_BC(CPU *cpu) {
    return ((uint16_t)cpu->B << 8) | cpu->C;
}

static void set_BC(CPU *cpu, uint16_t value) {
    cpu->B = (value >> 8) & 0xFF;
    cpu->C = value & 0xFF;
}

static uint16_t get_HL(CPU *cpu) {
    return ((uint16_t)cpu->H << 8) | cpu->L;
}

static void set_HL(CPU *cpu, uint16_t value) {
    cpu->H = (value >> 8) & 0xFF;
    cpu->L = value & 0xFF;
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
            int8_t off = (int8_t)mmu_read8(cpu->mmu, cpu->PC);
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
            int8_t off = (int8_t)mmu_read8(cpu->mmu, cpu->PC);
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
        case 0xF3: { // Disable Interrupts (DI)
            cpu->ime = 0;
            return 4;
        }
        case 0xE0: {
            uint8_t offset = mmu_read8(cpu->mmu, cpu->PC);
            cpu->PC++;

            uint16_t addr = 0xFF00 + offset;
            mmu_write8(cpu->mmu, addr, cpu->A);

            return 12;
        }
        case 0xCD: {
            uint8_t low = mmu_read8(cpu->mmu, cpu->PC);
            cpu->PC++;

            uint8_t high = mmu_read8(cpu->mmu, cpu->PC);
            cpu->PC++;

            uint16_t addr = ((uint16_t)high << 8) | low;

            // empurra PC atual na stack
            cpu->SP--;
            mmu_write8(cpu->mmu, cpu->SP, (cpu->PC >> 8) & 0xFF);

            cpu->SP--;
            mmu_write8(cpu->mmu, cpu->SP, cpu->PC & 0xFF);

            cpu->PC = addr;

            return 24;
        }
        case 0xF0: {
            uint8_t offset = mmu_read8(cpu->mmu, cpu->PC);
            cpu->PC++;

            uint16_t addr = 0xFF00 + offset;
            cpu->A = mmu_read8(cpu->mmu, addr);

            printf("LDH A,(a8): lendo addr=0x%04X valor=0x%02X\n", addr, cpu->A);

            return 12;
        }
        case 0x47: {
            cpu->B = cpu->A;
            return 4;
        }
        case 0x20: {
            int8_t off = (int8_t)mmu_read8(cpu->mmu, cpu->PC);
            cpu->PC++;

            if (!(cpu->F & 0x80)) { // Z desligado
                cpu->PC = (uint16_t)(cpu->PC + off);
                return 12;
            }

            return 8;
        }
        case 0xE6: {
            uint8_t n = mmu_read8(cpu->mmu, cpu->PC);
            cpu->PC++;

            cpu->A = cpu->A & n;

            cpu->F = 0x20; // H = 1
            if (cpu->A == 0) {
                cpu->F |= 0x80; // Z
            }

            return 8;
        }
        case 0x7F: {
            return 4;
        }
        case 0x78: {
            cpu->A = cpu->B;
            return 4;
        }
        case 0xC9: {
            uint8_t low = mmu_read8(cpu->mmu, cpu->SP);
            cpu->SP++;

            uint8_t high = mmu_read8(cpu->mmu, cpu->SP);
            cpu->SP++;

            cpu->PC = ((uint16_t)high << 8) | low;
            return 16;
        }
        case 0x31: {
            uint8_t low = mmu_read8(cpu->mmu, cpu->PC);
            cpu->PC++;

            uint8_t high = mmu_read8(cpu->mmu, cpu->PC);
            cpu->PC++;

            cpu->SP = ((uint16_t)high << 8) | low;
            return 12;
        }
        case 0xFF: {
            cpu->SP--;
            mmu_write8(cpu->mmu, cpu->SP, (cpu->PC >> 8) & 0xFF);

            cpu->SP--;
            mmu_write8(cpu->mmu, cpu->SP, cpu->PC & 0xFF);

            cpu->PC = 0x0038;
            return 16;
        }
        case 0x21: {
            uint8_t low = mmu_read8(cpu->mmu, cpu->PC);
            cpu->PC++;

            uint8_t high = mmu_read8(cpu->mmu, cpu->PC);
            cpu->PC++;

            cpu->L = low;
            cpu->H = high;

            return 12;
        }
        case 0xC0: {
            if (!(cpu->F & 0x80)) {
                uint8_t low = mmu_read8(cpu->mmu, cpu->SP);
                cpu->SP++;

                uint8_t high = mmu_read8(cpu->mmu, cpu->SP);
                cpu->SP++;

                cpu->PC = ((uint16_t)high << 8) | low;
                return 20;
            }

            return 8;
        }
        case 0x01: {
            uint8_t low = mmu_read8(cpu->mmu, cpu->PC);
            cpu->PC++;

            uint8_t high = mmu_read8(cpu->mmu, cpu->PC);
            cpu->PC++;

            cpu->C = low;
            cpu->B = high;

            return 12;
        }
        case 0x23: {
            uint16_t hl = ((uint16_t)cpu->H << 8) | cpu->L;
            hl++;

            cpu->H = (hl >> 8) & 0xFF;
            cpu->L = hl & 0xFF;

            return 8;
        }
        case 0x36: {
            uint8_t n = mmu_read8(cpu->mmu, cpu->PC);
            cpu->PC++;

            uint16_t hl = ((uint16_t)cpu->H << 8) | cpu->L;
            mmu_write8(cpu->mmu, hl, n);

            return 12;
        }
        case 0x0B: {
            uint16_t bc = ((uint16_t)cpu->B << 8) | cpu->C;
            bc--;

            cpu->B = (bc >> 8) & 0xFF;
            cpu->C = bc & 0xFF;

            return 8;
        }
        case 0xB1: {
            cpu->A = cpu->A | cpu->C;

            cpu->F = 0x00;
            if (cpu->A == 0) {
                cpu->F |= 0x80; // Z
            }

            return 4;
        }
        case 0xD5: {
            cpu->SP--;
            mmu_write8(cpu->mmu, cpu->SP, cpu->D);

            cpu->SP--;
            mmu_write8(cpu->mmu, cpu->SP, cpu->E);

            return 16;
        }

        case 0xCB: {
            uint8_t cb_opcode = mmu_read8(cpu->mmu, cpu->PC);
            cpu->PC++;

            switch (cb_opcode) {
                case 0x87: {
                    cpu->A &= 0xFE;
                    return 8;
                }

                default:
                    printf("CB opcode nao implementado: 0x%02X (PC=0x%04X)\n",
                           cb_opcode, (uint16_t)(cpu->PC - 2));
                    return 0;
            }
        }

        default: {
            printf("OPCODE nao implementado: 0x%02X (PC=0x%04X)\n",
                   opcode, (uint16_t)(cpu->PC - 1));
            return 0;
        }
    }
}