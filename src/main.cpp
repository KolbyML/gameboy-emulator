#include <iostream>
#include <vector>
#include <SDL.h>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <functional>
#include "main.h"
#include "memory_bus.h"
#include "cpu.h"
#include "utils.h"
#include "ppu.h"

bool DEBUG = false;

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

    memory_bus bus;
    PPU ppu(bus);
    bus.load_rom(file_dir);

    //uint8_t m_bootRom[256] = { 0x31,0xfe,0xff,0xaf,0x21,0xff,0x9f,0x32,0xcb,0x7c,0x20,0xfb,0x21,0x26,0xff,0xe,0x11,0x3e,0x80,0x32,0xe2,0xc,0x3e,0xf3,0xe2,0x32,0x3e,0x77,0x77,0x3e,0xfc,0xe0,0x47,0x11,0x4,0x1,0x21,0x10,0x80,0x1a,0xcd,0x95,0x0,0xcd,0x96,0x0,0x13,0x7b,0xfe,0x34,0x20,0xf3,0x11,0xd8,0x0,0x6,0x8,0x1a,0x13,0x22,0x23,0x5,0x20,0xf9,0x3e,0x19,0xea,0x10,0x99,0x21,0x2f,0x99,0xe,0xc,0x3d,0x28,0x8,0x32,0xd,0x20,0xf9,0x2e,0xf,0x18,0xf3,0x67,0x3e,0x64,0x57,0xe0,0x42,0x3e,0x91,0xe0,0x40,0x4,0x1e,0x2,0xe,0xc,0xf0,0x44,0xfe,0x90,0x20,0xfa,0xd,0x20,0xf7,0x1d,0x20,0xf2,0xe,0x13,0x24,0x7c,0x1e,0x83,0xfe,0x62,0x28,0x6,0x1e,0xc1,0xfe,0x64,0x20,0x6,0x7b,0xe2,0xc,0x3e,0x87,0xe2,0xf0,0x42,0x90,0xe0,0x42,0x15,0x20,0xd2,0x5,0x20,0x4f,0x16,0x20,0x18,0xcb,0x4f,0x6,0x4,0xc5,0xcb,0x11,0x17,0xc1,0xcb,0x11,0x17,0x5,0x20,0xf5,0x22,0x23,0x22,0x23,0xc9,0xce,0xed,0x66,0x66,0xcc,0xd,0x0,0xb,0x3,0x73,0x0,0x83,0x0,0xc,0x0,0xd,0x0,0x8,0x11,0x1f,0x88,0x89,0x0,0xe,0xdc,0xcc,0x6e,0xe6,0xdd,0xdd,0xd9,0x99,0xbb,0xbb,0x67,0x63,0x6e,0xe,0xec,0xcc,0xdd,0xdc,0x99,0x9f,0xbb,0xb9,0x33,0x3e,0x3c,0x42,0xb9,0xa5,0xb9,0xa5,0x42,0x3c,0x21,0x4,0x1,0x11,0xa8,0x0,0x1a,0x13,0xbe,0x20,0xfe,0x23,0x7d,0xfe,0x34,0x20,0xf5,0x6,0x19,0x78,0x86,0x23,0x5,0x20,0xfb,0x86,0x20,0xfe,0x3e,0x1,0xe0,0x50 };
    //bus.load_bootrom(m_bootRom);

    Flag_register flag_register = {};

    if (true) {
        registers.A = 0x01;
        flag_register.z = 1;
        flag_register.n = 0;
        flag_register.h = 1;
        flag_register.c = 1;
        registers.B = 0x00;
        registers.C = 0x13;
        registers.D = 0x00;
        registers.E = 0xD8;
        registers.H = 0x01;
        registers.L = 0x4D;
        registers.program_counter = 0x0100;
        registers.stack_pointer = 0xFFFE;
    }

    while (true) {
        SDL_Event e;
        if (SDL_WaitEvent(&e)) {
            if (e.type == SDL_QUIT)
                break;
        }
        if (bus.get_ime() && bus.get_count()) {
            auto call = [](memory_bus & bus, uint8_t address) {
                bus.set_ime_false();
                registers.stack_pointer -= 1;
                bus.set_memory(registers.stack_pointer, ((registers.program_counter) & 0xFF00) >> 8);
                registers.stack_pointer -= 1;
                bus.set_memory(registers.stack_pointer, ((registers.program_counter) & 0x00FF));
                registers.program_counter = address;
            };
            uint8_t IE = bus.get_memory(0xFFFF);
            uint8_t IF = bus.get_memory(0xFF0F);

            if (get_bit(IE, 0) && get_bit(IF, 0)) {
                bus.set_memory(0xFF0F, set_bit2(IF, 0, 0));
                call(bus, 0x40);
            } else if (get_bit(IE, 1) && get_bit(IF, 1)) {
                bus.set_memory(0xFF0F, set_bit2(IF, 1, 0));
                call(bus, 0x48);
            } else if (get_bit(IE, 2) && get_bit(IF, 2)) {
                bus.set_memory(0xFF0F, set_bit2(IF, 2, 0));
                call(bus, 0x50);
            } else if (get_bit(IE, 3) && get_bit(IF, 3)) {
                bus.set_memory(0xFF0F, set_bit2(IF, 3, 0));
                call(bus, 0x58);
            } else if (get_bit(IE, 4) && get_bit(IF, 4)) {
                bus.set_memory(0xFF0F, set_bit2(IF, 4, 0));
                call(bus, 0x60);
            }
        }
        bus.inc_count();
        uint8_t IE = bus.get_memory(0xFFFF);
        uint8_t IF = bus.get_memory(0xFF0F);
        uint8_t op_code = bus.get_memory(registers.program_counter);



        if (op_code != 0x76) {
            cpu_cycle(bus, flag_register);
            bus.increment_timer(bus.get_cycles());
        } else {
            if ((IE & IF) != 0) {
                registers.program_counter++;
            }

            bus.increment_timer(4);
        }
        ppu.draw();

        //    blarggs test - serial output

        if (true && bus.get_memory(0xff02) == 0x81) {
            char c = bus.get_memory(0xff01);
            printf("%c", c);
            bus.set_memory(0xff02, 0x0);
        }
    }
    finish:
    SDL_DestroyRenderer(ppu.renderer);
    SDL_DestroyWindow(ppu.window);
    SDL_Quit();
    return 0;
}
