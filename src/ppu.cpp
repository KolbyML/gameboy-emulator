//
// Created by kolby on 1/15/2023.
//

#include "ppu.h"
#include "utils.h"

void PPU::draw() {
    uint8_t LCDC = bus->get_memory(0xFF40);
    bool PPUEnabled = get_bit(LCDC, 7);
    bool WindowTileMapArea = get_bit(LCDC, 6);
    bool WindowEnabled = get_bit(LCDC, 5);
    bool BGWindowAddressingMode = get_bit(LCDC, 4);
    bool BGTileMapArea = get_bit(LCDC, 3);
    bool SpriteSize = get_bit(LCDC, 2);
    bool SpriteEnabled = get_bit(LCDC, 1);
    bool BGWindowPriority = get_bit(LCDC, 0);

    if (PPUEnabled) {
        // draw background

        if (BGWindowAddressingMode) {
            // uses signed addressing mode
            uint32_t base_pointer = 0x8800;

        } else {
            // uses signed addressing mode
            uint32_t base_pointer = 0x8000;

        }


        // Window enabled if true draw it
        if (WindowEnabled) {

        }

        // Sprites enabled if true draw it
        if (SpriteEnabled) {

        }
    }


    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
    for (int i = 0; i < 160; i++) {
        for (int k = 0; k < 144; k++) {
            if (screen[i][k] == 1) {
                SDL_RenderDrawPoint(renderer, i, k);
            }
        }
    }
    SDL_RenderPresent(renderer);
}

void PPU::set_draw_color(const uint32_t RGBA) {
    uint8_t A = (RGBA & 0xFF000000) >> 24;
    uint8_t R = (RGBA & 0xFF000000) >> 16;
    uint8_t G = (RGBA & 0xFF000000) >> 8;
    uint8_t B = RGBA & 0x000000FF;
    SDL_SetRenderDrawColor(renderer, A, R, G, B);
}