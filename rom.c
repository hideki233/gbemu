#include "rom.h"

#include <stdio.h>
#include <stdlib.h>

int rom_load(Rom *rom, const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) {
        printf("Erro ao tentar abrir o arquivo: %s\n", path);
        return 1;
    }

    if (fseek(f, 0, SEEK_END) != 0) {
        printf("Erro ao buscar o fim do arquivo\n");
        fclose(f);
        return 1;
    }

    long size = ftell(f);
    if (size <= 0) {
        printf("Tamanho de ROM invalido\n");
        fclose(f);
        return 1;
    }

    rewind(f);

    rom->data = (uint8_t*)malloc((size_t)size);
    if (!rom->data) {
        printf("Falha ao alocar memoria (%ld bytes)\n", size);
        fclose(f);
        return 1;
    }

    size_t lidos = fread(rom->data, 1, (size_t)size, f);
    fclose(f);

    if (lidos != (size_t)size) {
        printf("Leitura incompleta: %zu de %ld\n", lidos, size);
        free(rom->data);
        rom->data = NULL;
        rom->size = 0;
        return 1;
    }

    rom->size = (size_t)size;
    return 0;
}

void rom_free(Rom *rom){
    if (rom && rom->data){
        free(rom->data);
        rom->data = NULL;
        rom->size = 0;

    }


}  
void rom_print_header(const Rom *rom) {
    if (!rom || !rom->data || rom->size < 0x150) {
        printf("ROM invalida (muito pequena para ter header)\n");
        return;
    }

    const size_t TITLE_START = 0x0134;
    const size_t TITLE_LEN   = 16;

    char title[TITLE_LEN + 1];
    size_t i;

    for (i = 0; i < TITLE_LEN; i++) {
        uint8_t b = rom->data[TITLE_START + i];
        if (b == 0x00) break;
        title[i] = (char)b;
    }
    title[i] = '\0';

    printf("Titulo: %s\n", title);

    
    printf("Cartucho (0x0147): 0x%02X\n", rom->data[0x0147]);
    printf("ROM size  (0x0148): 0x%02X\n", rom->data[0x0148]);
    printf("RAM size  (0x0149): 0x%02X\n", rom->data[0x0149]);
}
