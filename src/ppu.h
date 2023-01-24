//
// Created by kolby on 1/15/2023.
//

#ifndef GAMEBOY_EMULATOR_PPU_H
#define GAMEBOY_EMULATOR_PPU_H

#include "memory_bus.h"
#include <SDL.h>

struct GameBoyColors {
    uint32_t color_1 = 	0xFF9bbc0f;
    uint32_t color_2 = 	0xFF8bac0f;
    uint32_t color_3 = 	0xFF306230;
    uint32_t color_4 = 	0xFF0f380f;
};

class PPU {
private:
    memory_bus *bus;
    bool screen[160][144];
    SDL_Event event;
    int scale = 4;
public:
    SDL_Renderer *renderer;
    SDL_Window *window;

public:
    PPU(memory_bus &bus) : bus(&bus) {
        for (int i = 0; i < 160; i++) {
            for (int k = 0; k < 144; k++) {
                screen[i][k] = 0;
            }
        }

        SDL_Init(SDL_INIT_VIDEO);

        window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 160 * scale, 144 * scale, 0);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        SDL_RenderSetLogicalSize(renderer, 160, 144);
        SDL_SetWindowTitle(window, "Kolby's GameBoy Emulator");
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
    };

    void draw();
    void set_draw_color(const uint32_t RGBA);
};

#endif //GAMEBOY_EMULATOR_PPU_H
