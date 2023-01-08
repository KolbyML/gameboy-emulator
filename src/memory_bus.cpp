//
// Created by kolby on 1/6/2023.
//

#include <vector>
#include <fstream>
#include <iostream>
#include "memory_bus.h"

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
    uint8_t op_code = memory.at(pointer);
    pointer++;
    return op_code;
}

uint8_t memory_bus::get_memory(const std::uint16_t &pointer) {
    return memory.at(pointer);
}

void memory_bus::set_memory(const uint16_t &pointer, const uint8_t &value) {
    memory.at(pointer) = value;
}