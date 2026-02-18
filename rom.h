#ifndef ROM_H
#define ROM_H

#include <stddef.h>
#include <stdint.h>

typedef struct{
    uint8_t *data;
    size_t size;

}Rom;

int rom_load(Rom *rom, const char *path);
void rom_free(Rom *rom);
void rom_print_header(const Rom *rom);

#endif