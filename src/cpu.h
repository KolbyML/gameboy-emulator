//
// Created by kolby on 2/20/2022.
//

#ifndef GAMEBOY_EMULATOR_CPU_H
#define GAMEBOY_EMULATOR_CPU_H

class Virtual_Register {
    uint8_t *x;
    uint8_t *y;
public:
    Virtual_Register(uint8_t &x, uint8_t &y) : x(&x), y(&y) {};

    std::uint16_t get() {
        return (*x << 8) + *y;
    }

    void set(std::uint16_t z) {
        *x = z >> 8;
        *y = z & 0x00FF;
    }

    void increment(std::int8_t z) {
        set(get() + z);
    }

    void decrement(std::int8_t z) {
        set(get() - z);
    }
};

struct Registers {
    uint8_t A;
    uint8_t B;
    uint8_t C;
    uint8_t D;
    uint8_t E;
    uint8_t F;
    uint8_t H;
    uint8_t L;
    std::uint16_t stack_pointer;
    std::uint16_t program_counter;
};

extern Registers registers;
extern Virtual_Register AF;
extern Virtual_Register BC;
extern Virtual_Register DE;
extern Virtual_Register HL;

struct Flag_register {
    uint8_t z;
    // This bit is set if and only if the result of an operation is zero. Used by conditional jumps.
    uint8_t n;
    uint8_t h;
    // https://gbdev.io/pandocs/CPU_Registers_and_Flags.html#the-bcd-flags-n-h
    uint8_t c;
    // https://gbdev.io/pandocs/CPU_Registers_and_Flags.html#the-carry-flag-c-or-cy
};

void cpu_cycle(memory_bus &bus, Flag_register &flag_register);
void set_flags(Flag_register &flag_register, std::int8_t Z, std::int8_t N, std::int8_t H, std::int8_t C);

#endif //GAMEBOY_EMULATOR_CPU_H
