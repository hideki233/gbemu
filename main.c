#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include "cpu.h"
#include "rom.h"
#include "mmu.h"

#define WIDTH 900
#define HEIGHT 600

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

    char GAME[17];
    for (int i = 0; i < 16; i++) {
        char c = mmu_read8(&mmu, 0x0134 + i);
        if (c == 0) {
            GAME[i] = '\0';
            break;
        }
        GAME[i] = c;
    }
    GAME[16] = '\0';
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erro SDL: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Window *window = SDL_CreateWindow(GAME,SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT,  0);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    int running = 1;
    SDL_Event event;

for (int i = 0; i < 1000000; i++) {
    cpu_step(&cpu);
}

while (running) {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = 0;
        }
    }
       // for (int i= 0; i < 10000; i++) {
      // //         int cycles = cpu_step(&cpu);
       //         if (cycles == 0) {
       //         printf("Parou na instrucao %d, PC=0x%04X\n", i, cpu.PC);
       //         break;
       //     }
       //     }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);




    for (int by = 0; by < 32; by++) {
        for (int bx = 0; bx < 32; bx++) {



            uint8_t lcdc = mmu_read8(&mmu, 0xFF40);
            uint16_t map_base = (lcdc & 0x08) ? 0x9C00 : 0x9800;
            uint16_t map_addr = map_base + (by * 32) + bx;
            uint8_t tile_index = mmu_read8(&mmu, map_addr);

            uint16_t tile_addr;

            if (lcdc & 0x10) {
                // modo 0x8000 (unsigned)
                tile_addr = 0x8000 + (tile_index * 16);
            } else {
                // modo 0x8800 (signed correto)
                tile_addr = 0x8800 + ((int8_t)tile_index + 128) * 16;
            }

            int tile_x = bx * 8;
            int tile_y = by * 8;
            if (by == 0 && bx < 10) {
                printf("%02X ", tile_index);
            }

            for (int y = 0; y < 8; y++) {

                uint8_t byte1 = mmu_read8(&mmu, tile_addr + y * 2);
                uint8_t byte2 = mmu_read8(&mmu, tile_addr + y * 2 + 1);

                for (int x = 0; x < 8; x++) {

                    int bit = 7 - x;

                    int color = ((byte1 >> bit) & 1) | (((byte2 >> bit) & 1) << 1);

                    switch (color) {
                        case 0: SDL_SetRenderDrawColor(renderer, 255,255,255,255); break;
                        case 1: SDL_SetRenderDrawColor(renderer, 170,170,170,255); break;
                        case 2: SDL_SetRenderDrawColor(renderer, 85,85,85,255); break;
                        case 3: SDL_SetRenderDrawColor(renderer, 0,0,0,255); break;
                    }

                    SDL_RenderDrawPoint(
                        renderer,
                        tile_x + x,
                        tile_y + y
                    );
                }
            }
        }
    }


    SDL_RenderPresent(renderer);
}


    printf("CPU terminou em PC=0x%04X, A=0x%02X, F=0x%02X\n", cpu.PC, cpu.A, cpu.F);
    // Imprime informações do header da ROM
    rom_print_header(&rom);

    rom_free(&rom);

    return 0;
}
