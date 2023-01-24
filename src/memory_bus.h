//
// Created by kolby on 1/6/2023.
//

#ifndef GAMEBOY_EMULATOR_MEMORY_BUS_H
#define GAMEBOY_EMULATOR_MEMORY_BUS_H

#include <vector>

class memory_bus {
private:
    std::vector<uint8_t> memory;
    bool IME;
    uint8_t count = 0;
    int timer_counter = 0;
    int cycle_counter = 0;
    int fetch_counter = 0;

public:
    memory_bus() : memory(0x10000, 0) {};

    bool load_bootrom(std::vector<uint8_t> buffer);

    bool load_rom(char *file_dir);

    uint8_t fetch(uint16_t &pointer);

    uint8_t get_memory(const uint16_t &pointer);

    void set_memory(const uint16_t &pointer, const uint8_t &value);

    void set_ime_true() {
        IME = true;
        count = 1;
    };

    void set_ei_ime_true() {
        IME = true;
        count = 0;
    };

    void set_ime_false() { IME = false; };

    bool get_ime() { return IME; };

    bool get_count() { return (bool) count; };

    void inc_count() { count++; };

    void increment_timer(int cycles);

    int get_cycles();
};

#endif //GAMEBOY_EMULATOR_MEMORY_BUS_H
