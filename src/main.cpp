#include <iostream>
#include <vector>
#include <SDL.h>
#include <cstdint>
#include <fstream>
#include "utils.h"
#include "main.h"
#include "cpu.h"

bool DEBUG = false;

void init_SDL2() {
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 160 * scale, 144 * scale, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_RenderSetLogicalSize(renderer, 160, 144);
    SDL_SetWindowTitle(window, "Kolby's GameBoy Emulator");
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
}

int main(int argc, char* argv[]) {
    char *file_dir;
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (i == 1) {
            file_dir = argv[i];
        } else if ((arg == "-d") || (arg == "--debug")) {
            DEBUG = true;
        } else if ((arg == "-h") || (arg == "--help")) {
            show_usage(argv[0]);
            return 0;
        }
    }

    // 4Kb of memory
    std::vector<std::uint8_t> memory(4096, 0);
    std::fill(memory.begin(), memory.end(), 0);

    std::ifstream input(file_dir, std::ios::binary);
    // copies all data into buffer
    std::vector<std::uint8_t> buffer(std::istreambuf_iterator<char>(input), {});

    init_SDL2();

    Flag_register flag_register = {};

    while (true) {
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            break;
        cpu_cycle(memory, registers, flag_register);
    }

    finish:
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
