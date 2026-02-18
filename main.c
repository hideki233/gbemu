#include <stdio.h>
#include "rom.h"

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

    rom_print_header(&rom);
    rom_free(&rom);

    return 0;
}
