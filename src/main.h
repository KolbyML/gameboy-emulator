#ifndef GAMEBOY_EMULATOR_MAIN_H
#define GAMEBOY_EMULATOR_MAIN_H
SDL_Event event;
SDL_Renderer *renderer;
SDL_Window *window;
int scale = 4;

void init_SDL2();
#endif //GAMEBOY_EMULATOR_MAIN_H
