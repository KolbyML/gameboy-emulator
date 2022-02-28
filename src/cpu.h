//
// Created by kolby on 2/20/2022.
//

#ifndef GAMEBOY_EMULATOR_CPU_H
#define GAMEBOY_EMULATOR_CPU_H

class Virtual_Register {
    std::uint8_t *x;
    std::uint8_t *y;
public:
    Virtual_Register(std::uint8_t &x, std::uint8_t &y) : x(&x), y(&y) {};

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
};

struct Registers {
    std::uint8_t A;
    std::uint8_t B;
    std::uint8_t C;
    std::uint8_t D;
    std::uint8_t E;
    std::uint8_t F;
    std::uint8_t H;
    std::uint8_t L;
    std::uint16_t stack_pointer;
    std::uint16_t program_counter;
};

static Registers registers = {};
static Virtual_Register AF = Virtual_Register(registers.A, registers.F);
static Virtual_Register BC = Virtual_Register(registers.B, registers.C);
static Virtual_Register DE = Virtual_Register(registers.D, registers.E);
static Virtual_Register HL = Virtual_Register(registers.H, registers.L);

struct Flag_register {
    std::uint8_t z;
    // This bit is set if and only if the result of an operation is zero. Used by conditional jumps.
    std::uint8_t n;
    std::uint8_t h;
    // https://gbdev.io/pandocs/CPU_Registers_and_Flags.html#the-bcd-flags-n-h
    std::uint8_t c;
    // https://gbdev.io/pandocs/CPU_Registers_and_Flags.html#the-carry-flag-c-or-cy
};

void cpu_cycle(std::vector<std::uint8_t> &memory, Registers &registers, Flag_register &flag_register);

#endif //GAMEBOY_EMULATOR_CPU_H
