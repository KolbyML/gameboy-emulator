//
// Created by kolby on 1/6/2023.
//

#include <vector>
#include <fstream>
#include <iostream>
#include "memory_bus.h"
#include "utils.h"
#include "cassert"

bool memory_bus::load_bootrom(std::vector<uint8_t> buffer) {
    for (int i = 0; i < buffer.size(); i++) {
        memory[i] = buffer[i];
    }

    // remove when I have PPU
    memory[0xFF44] = 0x90;
    return true;
}

bool memory_bus::load_rom(char *file_dir) {
    std::ifstream input(file_dir, std::ios::binary);
    // copies all data into buffer
    std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(input), {});
    for (int i = 0; i < buffer.size(); i++) {
        memory.at(i) = buffer[i];
    }
    // remove when I have PPU
    memory[0xFF44] = 0x90;
    return true;
}

uint8_t memory_bus::fetch(std::uint16_t &pointer) {
    fetch_counter++;
    uint8_t op_code = memory.at(pointer);
    pointer++;
    return op_code;
}

uint8_t memory_bus::get_memory(const std::uint16_t &pointer) {
    return memory.at(pointer);
}

void memory_bus::set_memory(const uint16_t &pointer, const uint8_t &value) {
    fetch_counter++;
    switch (pointer) {
        case 0xFF04:
            // Set div to 0 if a memory access occurs
            memory.at(0xFF04) = 0;
            break;
        default:
            memory.at(pointer) = value;
            break;
    }
}

void memory_bus::incement_timer(int cycles) {
    uint8_t DIV = get_memory(0xFF04);
    uint8_t TIMA = get_memory(0xFF05);
    uint8_t TMA = get_memory(0xFF06);
    uint8_t TAC = get_memory(0xFF07) & 0b11;

    // increment DIV
    cycle_counter += cycles;
    if (cycle_counter >= 0xFF) {
        cycle_counter = 0;
        DIV = (uint8_t) (DIV + 1);
        memory.at(0xFF04) = DIV;
    }

    if (get_bit(get_memory(0xFFFF), 2)) {
        timer_counter -= cycles;

        if (timer_counter <= 0) {
            if (TAC == 0) timer_counter = 1024;
            else if (TAC == 1) timer_counter = 16;
            else if (TAC == 2) timer_counter = 64;
            else if (TAC == 3) timer_counter = 256;
            else assert("Not supposed to be possible");

            if (TIMA == 0xFF) {
                // set TIMA to TMA
                set_memory(0xFF05, TMA);

                // requests a timer interrupt
                set_memory(0xFF0F, set_bit2(get_memory(0xFF0F), 2, 1));
            } else {
                set_memory(0xFF05, get_memory(0xFF05) + 1);
            }
        }
    }
}

int memory_bus::get_cycles() {
    int tmp = fetch_counter * 4;
    fetch_counter = 0;
    return tmp;
}
