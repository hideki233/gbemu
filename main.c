#include <stdio.h>
#include <stdint.h>

#include "cpu.h"
#include "rom.h"
#include "mmu.h"

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("Uso: gbemu [arquivo ROM]\n");
        return 1;
    }

    Rom rom = (Rom){0};

    if (rom_load(&rom, argv[1]) != 0) {
        printf("Nao foi possivel ler a rom %s\n", argv[1]);
        return 1;
    }

    MMU mmu;
    mmu_init(&mmu, &rom);
   printf("mmu criado\n");

    uint8_t b = mmu_read8(&mmu, 0x0134); // início do título no header
    printf("Primeiro byte do titulo (via mmu): 0x%02X\n", b);

    CPU cpu;
    cpu_init(&cpu, &mmu);

    for (int i= 0; i < 50; i++) {
        int cycles = cpu_step(&cpu);
        if (cycles == 0) break;
    }


    // Imprime informações do header da ROM
    rom_print_header(&rom);

    rom_free(&rom);

    return 0;
}
