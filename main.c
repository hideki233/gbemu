#include <stdio.h>
#include <stdint.h>
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
   printf("mmu criado");

    uint8_t b = mmu_read8(&mmu, 0x0134);
    printf("Primeiro byte do titulo (via mmu): 0x%02X\n", b);


    rom_print_header(&rom);
    rom_free(&rom);

    return 0;
}
