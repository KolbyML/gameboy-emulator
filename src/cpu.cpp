//
// Created by kolby on 2/20/2022.
//

#include <cstdint>
#include <vector>
#include <iostream>
#include "cpu.h"
#include "utils.h"

void set_flags(Flag_register &flag_register, std::int8_t Z, std::int8_t N, std::int8_t H, std::int8_t C) {
    if (Z != -1) flag_register.z = Z;
    if (N != -1) flag_register.n = N;
    if (H != -1) flag_register.h = H;
    if (C != -1) flag_register.c = C;
}

void SUB(std::vector<std::uint8_t> &memory, Registers &registers, Flag_register &flag_register) {
    switch (memory[registers.program_counter]) {
        case 0x90: {
            set_flags(flag_register, (registers.A - registers.B == 0), 1, is_borrow_from_bit4(registers.A, registers.B), registers.B > registers.A);
            registers.A = registers.A - registers.B;
            break;
        } case 0x91: {
            set_flags(flag_register, (registers.A - registers.C == 0), 1, is_borrow_from_bit4(registers.A, registers.C), registers.C > registers.A);
            registers.A = registers.A - registers.C;
            break;
        } case 0x92: {
            set_flags(flag_register, (registers.A - registers.D == 0), 1, is_borrow_from_bit4(registers.A, registers.D), registers.D > registers.A);
            registers.A = registers.A - registers.D;
            break;
        } case 0x93: {
            set_flags(flag_register, (registers.A - registers.E == 0), 1, is_borrow_from_bit4(registers.A, registers.E), registers.E > registers.A);
            registers.A = registers.A - registers.E;
            break;
        } case 0x94: {
            set_flags(flag_register, (registers.A - registers.H == 0), 1, is_borrow_from_bit4(registers.A, registers.H), registers.H > registers.A);
            registers.A = registers.A - registers.H;
            break;
        } case 0x95: {
            set_flags(flag_register, (registers.A - registers.L == 0), 1, is_borrow_from_bit4(registers.A, registers.L), registers.L > registers.A);
            registers.A = registers.A - registers.L;
            break;
        } case 0x96: {
            set_flags(flag_register, (registers.A - HL.get() == 0), 1, (registers.A & 0x08) < (HL.get() & 0x0008), HL.get() > registers.A);
            registers.A = registers.A - HL.get();
            break;
        } case 0x97: {
            set_flags(flag_register, (registers.A - registers.A == 0), 1, is_borrow_from_bit4(registers.A, registers.A), 0);
            registers.A = registers.A - registers.A;
            break;
        } case 0xD6: {
            std::uint8_t nn = memory[registers.program_counter + 1];
            set_flags(flag_register, (registers.A - nn == 0), 1, is_borrow_from_bit4(registers.A, nn), nn > registers.A);
            registers.A = registers.A - nn;
            break;
        }
        default:
            std::cout << "Unknown opcode: " << memory[registers.program_counter] << "\n";
            break;
    }
    return;
}

void SBC(std::vector<std::uint8_t> &memory, Registers &registers, Flag_register &flag_register) {
    switch (memory[registers.program_counter]) {
        case 0x90: {
            set_flags(flag_register, (registers.A - registers.B - flag_register.c == 0), 1, is_borrow_from_bit4(registers.A, registers.B - flag_register.c), registers.B + flag_register.c > registers.A);
            registers.A = registers.A - registers.B - flag_register.c;
            break;
        } case 0x91: {
            set_flags(flag_register, (registers.A - registers.C - flag_register.c == 0), 1, is_borrow_from_bit4(registers.A, registers.C - flag_register.c), registers.C + flag_register.c > registers.A);
            registers.A = registers.A - registers.C - flag_register.c;
            break;
        } case 0x92: {
            set_flags(flag_register, (registers.A - registers.D - flag_register.c == 0), 1, is_borrow_from_bit4(registers.A, registers.D - flag_register.c), registers.D + flag_register.c > registers.A);
            registers.A = registers.A - registers.D - flag_register.c;
            break;
        } case 0x93: {
            set_flags(flag_register, (registers.A - registers.E - flag_register.c == 0), 1, is_borrow_from_bit4(registers.A, registers.E - flag_register.c), registers.E + flag_register.c > registers.A);
            registers.A = registers.A - registers.E - flag_register.c;
            break;
        } case 0x94: {
            set_flags(flag_register, (registers.A - registers.H - flag_register.c == 0), 1, is_borrow_from_bit4(registers.A, registers.H - flag_register.c), registers.H + flag_register.c > registers.A);
            registers.A = registers.A - registers.H - flag_register.c;
            break;
        } case 0x95: {
            set_flags(flag_register, (registers.A - registers.L - flag_register.c == 0), 1, is_borrow_from_bit4(registers.A, registers.L - flag_register.c), registers.L + flag_register.c > registers.A);
            registers.A = registers.A - registers.L - flag_register.c;
            break;
        } case 0x96: {
            set_flags(flag_register, (registers.A - HL.get() - flag_register.c == 0), 1, (registers.A & 0x08) < ((HL.get()  - flag_register.c) & 0x0008), HL.get() + flag_register.c > registers.A);
            registers.A = registers.A - HL.get() - flag_register.c;
            break;
        } case 0x97: {
            set_flags(flag_register, (registers.A - registers.A - flag_register.c == 0), 1, is_borrow_from_bit4(registers.A, registers.A - flag_register.c), registers.A + flag_register.c > registers.A);
            registers.A = registers.A - registers.A - flag_register.c;
            break;
        } case 0xDE: {
            std::uint8_t nn = memory[registers.program_counter + 1];
            set_flags(flag_register, (registers.A - nn - flag_register.c == 0), 1, is_borrow_from_bit4(registers.A, nn - flag_register.c), nn + flag_register.c > registers.A);
            registers.A = registers.A - nn - flag_register.c;
            break;
        }
        default:
            std::cout << "Unknown opcode: " << memory[registers.program_counter] << "\n";
            break;
    }
    return;
}

void CP(std::vector<std::uint8_t> &memory, Registers &registers, Flag_register &flag_register) {
    switch (memory[registers.program_counter]) {
        case 0xB8: {
            set_flags(flag_register, (registers.A - registers.B == 0), 1, is_borrow_from_bit4(registers.A, registers.B), registers.B > registers.A);
            break;
        } case 0xB9: {
            set_flags(flag_register, (registers.A - registers.C == 0), 1, is_borrow_from_bit4(registers.A, registers.C), registers.C > registers.A);
            break;
        } case 0xBA: {
            set_flags(flag_register, (registers.A - registers.D == 0), 1, is_borrow_from_bit4(registers.A, registers.D), registers.D > registers.A);
            break;
        } case 0xBB: {
            set_flags(flag_register, (registers.A - registers.E == 0), 1, is_borrow_from_bit4(registers.A, registers.E), registers.E > registers.A);
            break;
        } case 0xBC: {
            set_flags(flag_register, (registers.A - registers.H == 0), 1, is_borrow_from_bit4(registers.A, registers.H), registers.H > registers.A);
            break;
        } case 0xBD: {
            set_flags(flag_register, (registers.A - registers.L == 0), 1, is_borrow_from_bit4(registers.A, registers.L), registers.L > registers.A);
            break;
        } case 0xBE: {
            set_flags(flag_register, (registers.A - HL.get() == 0), 1, (registers.A & 0x08) < (HL.get() & 0x0008), HL.get() > registers.A);
            break;
        } case 0xBF: {
            set_flags(flag_register, (registers.A - registers.A == 0), 1, is_borrow_from_bit4(registers.A, registers.A), 0);
            break;
        } case 0xFE: {
            std::uint8_t nn = memory[registers.program_counter + 1];
            set_flags(flag_register, (registers.A - nn == 0), 1, is_borrow_from_bit4(registers.A, nn), nn > registers.A);
            break;
        }
        default:
            std::cout << "Unknown opcode: " << memory[registers.program_counter] << "\n";
            break;
    }
    return;
}

void ADD(std::vector<std::uint8_t> &memory, Registers &registers, Flag_register &flag_register) {
    switch (memory[registers.program_counter]) {
        case 0x09: {
            set_flags(flag_register, -1, 0, is_half_carry_u16(HL.get(), BC.get()), is_carry_u16(HL.get(), BC.get()));
            HL.set(HL.get() + BC.get());
            break;
        } case 0x19: {
            set_flags(flag_register, -1, 0, is_half_carry_u16(HL.get(), DE.get()), is_carry_u16(HL.get(), DE.get()));
            HL.set(HL.get() + DE.get());
            break;
        } case 0x29: {
            set_flags(flag_register, -1, 0, is_half_carry_u16(HL.get(), HL.get()), is_carry_u16(HL.get(), HL.get()));
            HL.set(HL.get() + HL.get());
            break;
        } case 0x39: {
            set_flags(flag_register, -1, 0, is_half_carry_u16(HL.get(), registers.stack_pointer), is_carry_u16(HL.get(), registers.stack_pointer));
            HL.set(HL.get() + registers.stack_pointer);
            break;
        } case 0x80: {
            set_flags(flag_register, (registers.A + registers.B == 0), 0, is_half_carry_u8(registers.A, registers.B), is_carry_u8(registers.A, registers.B));
            registers.A = registers.A + registers.B;
            break;
        } case 0x81: {
            set_flags(flag_register, (registers.A + registers.C == 0), 0, is_half_carry_u8(registers.A, registers.C), is_carry_u8(registers.A, registers.C));
            registers.A = registers.A + registers.C;
            break;
        } case 0x82: {
            set_flags(flag_register, (registers.A + registers.D == 0), 0, is_half_carry_u8(registers.A, registers.D), is_carry_u8(registers.A, registers.D));
            registers.A = registers.A + registers.D;
            break;
        } case 0x83: {
            set_flags(flag_register, (registers.A + registers.E == 0), 0, is_half_carry_u8(registers.A, registers.E), is_carry_u8(registers.A, registers.E));
            registers.A = registers.A + registers.E;
            break;
        } case 0x84: {
            set_flags(flag_register, (registers.A + registers.H == 0), 0, is_half_carry_u8(registers.A, registers.H), is_carry_u8(registers.A, registers.H));
            registers.A = registers.A + registers.H;
            break;
        } case 0x85: {
            set_flags(flag_register, (registers.A + registers.L == 0), 0, is_half_carry_u8(registers.A, registers.L), is_carry_u8(registers.A, registers.L));
            registers.A = registers.A + registers.L;
            break;
        } case 0x86: {
            set_flags(flag_register, (registers.A + HL.get() == 0), 0, is_half_carry_u8(registers.A, HL.get()), is_carry_u8_u16(registers.A, HL.get()));
            registers.A = registers.A + HL.get();
            break;
        } case 0x87: {
            set_flags(flag_register, (registers.A + registers.A == 0), 0, is_half_carry_u8(registers.A, registers.A), is_carry_u8(registers.A, registers.A));
            registers.A = registers.A + registers.A;
            break;
        } case 0xC6: {
            std::uint8_t nn = memory[registers.program_counter + 1];
            set_flags(flag_register, (registers.A + nn == 0), 0, is_half_carry_u8(registers.A, nn), is_carry_u8(registers.A, nn));
            registers.A = registers.A + nn;
            break;
        } case 0xE8: {
            std::int8_t nn = memory[registers.program_counter + 1];
            set_flags(flag_register, 0, 0, is_half_carry_i8(registers.A, nn), is_carry_i8(registers.A, nn));
            registers.stack_pointer = registers.stack_pointer + nn;
            break;
        }
        default:
            std::cout << "Unknown opcode: " << memory[registers.program_counter] << "\n";
            break;
    }
    return;
}

void AND(std::vector<std::uint8_t> &memory, Registers &registers, Flag_register &flag_register) {
    switch (memory[registers.program_counter]) {
        case 0xA0: {
            set_flags(flag_register, (registers.A & registers.B == 0), 0, 1, 0);
            registers.A = registers.A & registers.B;
            break;
        } case 0xA1: {
            set_flags(flag_register, (registers.A & registers.C == 0), 0, 1, 0);
            registers.A = registers.A & registers.C;
            break;
        } case 0xA2: {
            set_flags(flag_register, (registers.A & registers.D == 0), 0, 1, 0);
            registers.A = registers.A & registers.D;
            break;
        } case 0xA3: {
            set_flags(flag_register, (registers.A & registers.E == 0), 0, 1, 0);
            registers.A = registers.A & registers.E;
            break;
        } case 0xA4: {
            set_flags(flag_register, (registers.A & registers.H == 0), 0, 1, 0);
            registers.A = registers.A & registers.H;
            break;
        } case 0xA5: {
            set_flags(flag_register, (registers.A & registers.L == 0), 0, 1, 0);
            registers.A = registers.A & registers.L;
            break;
        } case 0xA6: {
            set_flags(flag_register, (registers.A & HL.get() == 0), 0, 1, 0);
            registers.A = registers.A & HL.get();
            break;
        } case 0xA7: {
            set_flags(flag_register, (registers.A & registers.A == 0), 0, 1, 0);
            registers.A = registers.A & registers.A;
            break;
        } case 0xE6: {
            std::uint8_t nn = memory[registers.program_counter + 1];
            set_flags(flag_register, (registers.A & nn == 0), 0, 1, 0);
            registers.A = registers.A & nn;
            break;
        }
        default:
            std::cout << "Unknown opcode: " << memory[registers.program_counter] << "\n";
            return;
    }
    return;
}

void XOR(std::vector<std::uint8_t> &memory, Registers &registers, Flag_register &flag_register) {
    switch (memory[registers.program_counter]) {
        case 0xA8: {
            set_flags(flag_register, (registers.A ^ registers.B == 0), 0, 0, 0);
            registers.A = registers.A ^ registers.B;
            break;
        } case 0xA9: {
            set_flags(flag_register, (registers.A ^ registers.C == 0), 0, 0, 0);
            registers.A = registers.A ^ registers.C;
            break;
        } case 0xAA: {
            set_flags(flag_register, (registers.A ^ registers.D == 0), 0, 0, 0);
            registers.A = registers.A ^ registers.D;
            break;
        } case 0xAB: {
            set_flags(flag_register, (registers.A ^ registers.E == 0), 0, 0, 0);
            registers.A = registers.A ^ registers.E;
            break;
        } case 0xAC: {
            set_flags(flag_register, (registers.A ^ registers.H == 0), 0, 0, 0);
            registers.A = registers.A ^ registers.H;
            break;
        } case 0xAD: {
            set_flags(flag_register, (registers.A ^ registers.L == 0), 0, 0, 0);
            registers.A = registers.A ^ registers.L;
            break;
        } case 0xAE: {
            set_flags(flag_register, (registers.A ^ HL.get() == 0), 0, 0, 0);
            registers.A = registers.A ^ HL.get();
            break;
        } case 0xAF: {
            set_flags(flag_register, (registers.A ^ registers.A == 0), 0, 0, 0);
            registers.A = registers.A ^ registers.A;
            break;
        } case 0xEE: {
            std::uint8_t nn = memory[registers.program_counter + 1];
            set_flags(flag_register, (registers.A ^ nn == 0), 0, 0, 0);
            registers.A = registers.A ^ nn;
            break;
        }
        default:
            std::cout << "Unknown opcode: " << memory[registers.program_counter] << "\n";
            return;
    }
    return;
}

void OR(std::vector<std::uint8_t> &memory, Registers &registers, Flag_register &flag_register) {
    switch (memory[registers.program_counter]) {
        case 0xB0: {
            set_flags(flag_register, (registers.A | registers.B == 0), 0, 0, 0);
            registers.A = registers.A | registers.B;
            break;
        } case 0xB1: {
            set_flags(flag_register, (registers.A | registers.C == 0), 0, 0, 0);
            registers.A = registers.A | registers.C;
            break;
        } case 0xB2: {
            set_flags(flag_register, (registers.A | registers.D == 0), 0, 0, 0);
            registers.A = registers.A | registers.D;
            break;
        } case 0xB3: {
            set_flags(flag_register, (registers.A | registers.E == 0), 0, 0, 0);
            registers.A = registers.A | registers.E;
            break;
        } case 0xB4: {
            set_flags(flag_register, (registers.A | registers.H == 0), 0, 0, 0);
            registers.A = registers.A | registers.H;
            break;
        } case 0xB5: {
            set_flags(flag_register, (registers.A | registers.L == 0), 0, 0, 0);
            registers.A = registers.A | registers.L;
            break;
        } case 0xB6: {
            set_flags(flag_register, (registers.A | HL.get() == 0), 0, 0, 0);
            registers.A = registers.A | HL.get();
            break;
        } case 0xB7: {
            set_flags(flag_register, (registers.A | registers.A == 0), 0, 0, 0);
            registers.A = registers.A | registers.A;
            break;
        } case 0xF6: {
            std::uint8_t nn = memory[registers.program_counter + 1];
            set_flags(flag_register, (registers.A | nn == 0), 0, 0, 0);
            registers.A = registers.A | nn;
            break;
        }
        default:
            std::cout << "Unknown opcode: " << memory[registers.program_counter] << "\n";
            return;
    }
    return;
}

void ADC(std::vector<std::uint8_t> &memory, Registers &registers, Flag_register &flag_register) {
    switch (memory[registers.program_counter]) {
        case 0x88: {
            set_flags(flag_register, (registers.A + registers.B + flag_register.c == 0), 0, is_half_carry_u8(registers.A, registers.B, flag_register.c), is_carry_u8(registers.A, registers.B, flag_register.c));
            registers.A = registers.A + registers.B + flag_register.c;
            break;
        } case 0x89: {
            set_flags(flag_register, (registers.A + registers.C + flag_register.c == 0), 0, is_half_carry_u8(registers.A, registers.C, flag_register.c), is_carry_u8(registers.A, registers.C, flag_register.c));
            registers.A = registers.A + registers.C + flag_register.c;
            break;
        } case 0x8A: {
            set_flags(flag_register, (registers.A + registers.D + flag_register.c == 0), 0, is_half_carry_u8(registers.A, registers.D, flag_register.c), is_carry_u8(registers.A, registers.D, flag_register.c));
            registers.A = registers.A + registers.D + flag_register.c;
            break;
        } case 0x8B: {
            set_flags(flag_register, (registers.A + registers.E + flag_register.c == 0), 0, is_half_carry_u8(registers.A, registers.E, flag_register.c), is_carry_u8(registers.A, registers.E, flag_register.c));
            registers.A = registers.A + registers.E + flag_register.c;
            break;
        } case 0x8C: {
            set_flags(flag_register, (registers.A + registers.H + flag_register.c == 0), 0, is_half_carry_u8(registers.A, registers.H, flag_register.c), is_carry_u8(registers.A, registers.H, flag_register.c));
            registers.A = registers.A + registers.H + flag_register.c;
            break;
        } case 0x8D: {
            set_flags(flag_register, (registers.A + registers.L + flag_register.c == 0), 0, is_half_carry_u8(registers.A, registers.L, flag_register.c), is_carry_u8(registers.A, registers.L, flag_register.c));
            registers.A = registers.A + registers.L + flag_register.c;
            break;
        } case 0x8E: {
            set_flags(flag_register, (registers.A + HL.get() + flag_register.c == 0), 0, is_half_carry_u8(registers.A, HL.get(), flag_register.c), is_carry_u8_u16(registers.A, HL.get(), flag_register.c));
            registers.A = registers.A + HL.get() + flag_register.c;
            break;
        } case 0x8F: {
            set_flags(flag_register, (registers.A + registers.A + flag_register.c == 0), 0, is_half_carry_u8(registers.A, registers.A, flag_register.c), is_carry_u8(registers.A, registers.A, flag_register.c));
            registers.A = registers.A + registers.A + flag_register.c;
            break;
        } case 0xCE: {
            std::uint8_t nn = memory[registers.program_counter + 1];
            set_flags(flag_register, (registers.A + nn + flag_register.c == 0), 0, is_half_carry_u8(registers.A, nn, flag_register.c), is_carry_u8(registers.A, nn, flag_register.c));
            registers.A = registers.A + nn + flag_register.c;
            break;
        }
        default:
            std::cout << "Unknown opcode: " << memory[registers.program_counter] << "\n";
            break;
    }
    return;
}

void LOAD(std::vector<std::uint8_t> &memory, Registers &registers, Flag_register &flag_register) {
    switch (memory[registers.program_counter]) {
        case 0x01: {
            BC.set(Virtual_Register(memory[registers.program_counter + 1], memory[registers.program_counter + 2]).get());
            break;
        } case 0x11: {
            DE.set(Virtual_Register(memory[registers.program_counter + 1], memory[registers.program_counter + 2]).get());
            break;
        } case 0x21: {
            HL.set(Virtual_Register(memory[registers.program_counter + 1], memory[registers.program_counter + 2]).get());
            break;
        } case 0x31: {
            registers.stack_pointer = Virtual_Register(memory[registers.program_counter + 1], memory[registers.program_counter + 2]).get();
            break;
        } case 0x02: {
            BC.set(registers.A);
            break;
        } case 0x12: {
            DE.set(registers.A);
            break;
        } case 0x22: {
            HL.set(registers.A);
            HL.increment(1);
            break;
        } case 0x32: {
            BC.set(registers.A);
            HL.increment(-1);
            break;
        } case 0x06: {
            std::uint8_t nn = memory[registers.program_counter + 1];
            registers.B = nn;
            break;
        } case 0x16: {
            std::uint8_t nn = memory[registers.program_counter + 1];
            registers.D = nn;
            break;
        } case 0x26: {
            std::uint8_t nn = memory[registers.program_counter + 1];
            registers.H = nn;
            break;
        } case 0x36: {
            std::uint8_t nn = memory[registers.program_counter + 1];
            HL.set(nn);
            break;
        } case 0x0A: {
            registers.A = BC.get();
            break;
        } case 0x1A: {
            registers.A = DE.get();
            break;
        } case 0x2A: {
            registers.A = HL.get();
            HL.increment(1);
            break;
        } case 0x3A: {
            registers.A = HL.get();
            HL.increment(-1);
            break;
        } case 0x0E: {
            std::uint8_t nn = memory[registers.program_counter + 1];
            registers.C = nn;
            break;
        } case 0x1E: {
            std::uint8_t nn = memory[registers.program_counter + 1];
            registers.E = nn;
            break;
        } case 0x2E: {
            std::uint8_t nn = memory[registers.program_counter + 1];
            registers.L = nn;
            break;
        } case 0x3E: {
            std::uint8_t nn = memory[registers.program_counter + 1];
            registers.A = nn;
            break;
        } case 0x08: {
            Virtual_Register(memory[registers.program_counter + 1], memory[registers.program_counter + 2]).set(registers.stack_pointer);
            break;
        } case 0x40: {
            registers.B = registers.B;
            break;
        } case 0x41: {
            registers.B = registers.C;
            break;
        } case 0x42: {
            registers.B = registers.D;
            break;
        } case 0x43: {
            registers.B = registers.E;
            break;
        } case 0x44: {
            registers.B = registers.H;
            break;
        } case 0x45: {
            registers.B = registers.L;
            break;
        } case 0x46: {
            registers.B = HL.get();
            break;
        } case 0x47: {
            registers.B = registers.A;
            break;
        } case 0x48: {
            registers.C = registers.B;
            break;
        } case 0x49: {
            registers.C = registers.C;
            break;
        } case 0x4A: {
            registers.C = registers.D;
            break;
        } case 0x4B: {
            registers.C = registers.E;
            break;
        } case 0x4C: {
            registers.C = registers.H;
            break;
        } case 0x4D: {
            registers.C = registers.L;
            break;
        } case 0x4E: {
            registers.C = HL.get();
            break;
        } case 0x4F: {
            registers.C = registers.A;
            break;
        } case 0x50: {
            registers.D = registers.B;
            break;
        } case 0x51: {
            registers.D = registers.C;
            break;
        } case 0x52: {
            registers.D = registers.D;
            break;
        } case 0x53: {
            registers.D = registers.E;
            break;
        } case 0x54: {
            registers.D = registers.H;
            break;
        } case 0x55: {
            registers.D = registers.L;
            break;
        } case 0x56: {
            registers.D = HL.get();
            break;
        } case 0x57: {
            registers.D = registers.A;
            break;
        } case 0x58: {
            registers.E = registers.B;
            break;
        } case 0x59: {
            registers.E = registers.C;
            break;
        } case 0x5A: {
            registers.E = registers.D;
            break;
        } case 0x5B: {
            registers.E = registers.E;
            break;
        } case 0x5C: {
            registers.E = registers.H;
            break;
        } case 0x5D: {
            registers.E = registers.L;
            break;
        } case 0x5E: {
            registers.E = HL.get();
            break;
        } case 0x5F: {
            registers.E = registers.A;
            break;
        } case 0x60: {
            registers.H = registers.B;
            break;
        } case 0x61: {
            registers.H = registers.C;
            break;
        } case 0x62: {
            registers.H = registers.D;
            break;
        } case 0x63: {
            registers.H = registers.E;
            break;
        } case 0x64: {
            registers.H = registers.H;
            break;
        } case 0x65: {
            registers.H = registers.L;
            break;
        } case 0x66: {
            registers.H = HL.get();
            break;
        } case 0x67: {
            registers.H = registers.A;
            break;
        } case 0x68: {
            registers.L = registers.B;
            break;
        } case 0x69: {
            registers.L = registers.C;
            break;
        } case 0x6A: {
            registers.L = registers.D;
            break;
        } case 0x6B: {
            registers.L = registers.E;
            break;
        } case 0x6C: {
            registers.L = registers.H;
            break;
        } case 0x6D: {
            registers.L = registers.L;
            break;
        } case 0x6E: {
            registers.L = HL.get();
            break;
        } case 0x6F: {
            registers.L = registers.A;
            break;
        } case 0x70: {
            // I am so confused how this is useful
            HL.set(registers.B);
            break;
        } case 0x71: {
            HL.set(registers.C);
            break;
        } case 0x72: {
            HL.set(registers.D);
            break;
        } case 0x73: {
            HL.set(registers.E);
            break;
        } case 0x74: {
            HL.set(registers.H);
            break;
        } case 0x75: {
            HL.set(registers.L);
            break;
        } case 0x77: {
            HL.set(registers.A);
            break;
        } case 0x78: {
            registers.A = registers.B;
            break;
        } case 0x79: {
            registers.A = registers.C;
            break;
        } case 0x7A: {
            registers.A = registers.D;
            break;
        } case 0x7B: {
            registers.A = registers.E;
            break;
        } case 0x7C: {
            registers.A = registers.H;
            break;
        } case 0x7D: {
            registers.A = registers.L;
            break;
        } case 0x7E: {
            registers.A = HL.get();
            break;
        } case 0x7F: {
            registers.A = registers.A;
            break;
        } case 0xE0: {
            std::uint8_t nn = memory[registers.program_counter + 1];
            memory[0xFF00 + nn] = registers.A;
            break;
        } case 0xE2: {
            memory[0xFF00 + registers.C] = registers.A;
            break;
        } case 0xEA: {
            // not sure if I implemented this right
            memory[registers.program_counter + 1] = registers.A;
            break;
        } case 0xF0: {
            std::uint8_t nn = memory[registers.program_counter + 1];
            registers.A = memory[0xFF00 + nn];
            break;
        } case 0xF2: {
            registers.A = memory[0xFF00 + registers.C];
            break;
        } case 0xF8: {
            std::int8_t nn = memory[registers.program_counter + 1];
            set_flags(flag_register, 0, 0, (((registers.stack_pointer & 0x0F) + (nn & 0x0F)) & 0x10) == 0b00010000, (((std::uint16_t)registers.stack_pointer + nn) & 0x100) == 0b100000000);
            HL.set(registers.stack_pointer + nn);
            break;
        } case 0xF9: {
            registers.stack_pointer = HL.get();
            break;
        } case 0xFA: {
            // not sure if I implemented this right
            registers.A = memory[registers.program_counter + 1];
            break;
        }
        default:
            std::cout << "Unknown opcode: " << memory[registers.program_counter] << "\n";
            break;
    }
    return;
}

void RLC(std::vector<std::uint8_t> &memory, Registers &registers, Flag_register &flag_register) {
    switch (memory[registers.program_counter + 1]) {
        case 0x00: {
            set_flags(flag_register, registers.B == 0, 0, 0, get_bit(registers.B, 7));
            rotate_left(registers.B);
            break;
        } case 0x01: {
            set_flags(flag_register, registers.C == 0, 0, 0, get_bit(registers.C, 7));
            rotate_left(registers.C);
            break;
        } case 0x02: {
            set_flags(flag_register, registers.D == 0, 0, 0, get_bit(registers.D, 7));
            rotate_left(registers.D);
            break;
        } case 0x03: {
            set_flags(flag_register, registers.E == 0, 0, 0, get_bit(registers.E, 7));
            rotate_left(registers.E);
            break;
        } case 0x04: {
            set_flags(flag_register, registers.H == 0, 0, 0, get_bit(registers.H, 7));
            rotate_left(registers.H);
            break;
        } case 0x05: {
            set_flags(flag_register, registers.L == 0, 0, 0, get_bit(registers.L, 7));
            rotate_left(registers.L);
            break;
        } case 0x06: {
            set_flags(flag_register, memory[HL.get()] == 0, 0, 0, get_bit(memory[HL.get()], 7));
            rotate_left(memory[HL.get()]);
            break;
        } case 0x07: {
            set_flags(flag_register, registers.A == 0, 0, 0, get_bit(registers.A, 7));
            rotate_left(registers.A);
            break;
        }
        default:
            std::cout << "Unknown opcode: " << memory[registers.program_counter] << "\n";
            break;
    }
    return;
}

void RRC(std::vector<std::uint8_t> &memory, Registers &registers, Flag_register &flag_register) {
    switch (memory[registers.program_counter + 1]) {
        case 0x08: {
            set_flags(flag_register, registers.B == 0, 0, 0, get_bit(registers.B, 0));
            rotate_right(registers.B);
            break;
        } case 0x09: {
            set_flags(flag_register, registers.C == 0, 0, 0, get_bit(registers.C, 0));
            rotate_right(registers.C);
            break;
        } case 0x0A: {
            set_flags(flag_register, registers.D == 0, 0, 0, get_bit(registers.D, 0));
            rotate_right(registers.D);
            break;
        } case 0x0B: {
            set_flags(flag_register, registers.E == 0, 0, 0, get_bit(registers.E, 0));
            rotate_right(registers.E);
            break;
        } case 0x0C: {
            set_flags(flag_register, registers.H == 0, 0, 0, get_bit(registers.H, 0));
            rotate_right(registers.H);
            break;
        } case 0x0D: {
            set_flags(flag_register, registers.L == 0, 0, 0, get_bit(registers.L, 0));
            rotate_right(registers.L);
            break;
        } case 0x0E: {
            set_flags(flag_register, memory[HL.get()] == 0, 0, 0, get_bit(memory[HL.get()], 0));
            rotate_right(memory[HL.get()]);
            break;
        } case 0x0F: {
            set_flags(flag_register, registers.A == 0, 0, 0, get_bit(registers.A, 0));
            rotate_right(registers.A);
            break;
        }
        default:
            std::cout << "Unknown opcode: " << memory[registers.program_counter] << "\n";
            break;
    }
    return;
}

void RL(std::vector<std::uint8_t> &memory, Registers &registers, Flag_register &flag_register) {
    switch (memory[registers.program_counter + 1]) {
        case 0x10: {
            std::uint8_t carry = flag_register.c;
            set_flags(flag_register, registers.B == 0, 0, 0, get_bit(registers.B, 7));
            rotate_left(registers.B);
            set_bit(registers.B, 0, carry);
            break;
        } case 0x11: {
            std::uint8_t carry = flag_register.c;
            set_flags(flag_register, registers.C == 0, 0, 0, get_bit(registers.C, 7));
            rotate_left(registers.C);
            set_bit(registers.C, 0, carry);
            break;
        } case 0x12: {
            std::uint8_t carry = flag_register.c;
            set_flags(flag_register, registers.D == 0, 0, 0, get_bit(registers.D, 7));
            rotate_left(registers.D);
            set_bit(registers.D, 0, carry);
            break;
        } case 0x13: {
            std::uint8_t carry = flag_register.c;
            set_flags(flag_register, registers.E == 0, 0, 0, get_bit(registers.E, 7));
            rotate_left(registers.E);
            set_bit(registers.E, 0, carry);
            break;
        } case 0x14: {
            std::uint8_t carry = flag_register.c;
            set_flags(flag_register, registers.H == 0, 0, 0, get_bit(registers.H, 7));
            rotate_left(registers.H);
            set_bit(registers.H, 0, carry);
            break;
        } case 0x15: {
            std::uint8_t carry = flag_register.c;
            set_flags(flag_register, registers.L == 0, 0, 0, get_bit(registers.L, 7));
            rotate_left(registers.L);
            set_bit(registers.L, 0, carry);
            break;
        } case 0x16: {
            std::uint8_t carry = flag_register.c;
            set_flags(flag_register, memory[HL.get()] == 0, 0, 0, get_bit(memory[HL.get()], 7));
            rotate_left(memory[HL.get()]);
            set_bit(memory[HL.get()], 0, carry);
            break;
        } case 0x17: {
            std::uint8_t carry = flag_register.c;
            set_flags(flag_register, registers.A == 0, 0, 0, get_bit(registers.A, 7));
            rotate_left(registers.A);
            set_bit(registers.A, 0, carry);
            break;
        }
        default:
            std::cout << "Unknown opcode: " << memory[registers.program_counter] << "\n";
            break;
    }
    return;
}

void RR(std::vector<std::uint8_t> &memory, Registers &registers, Flag_register &flag_register) {
    switch (memory[registers.program_counter + 1]) {
        case 0x18: {
            std::uint8_t carry = flag_register.c;
            set_flags(flag_register, registers.B == 0, 0, 0, get_bit(registers.B, 0));
            rotate_right(registers.B);
            set_bit(registers.B, 7, carry);
            break;
        } case 0x19: {
            std::uint8_t carry = flag_register.c;
            set_flags(flag_register, registers.C == 0, 0, 0, get_bit(registers.C, 0));
            rotate_right(registers.C);
            set_bit(registers.C, 7, carry);
            break;
        } case 0x1A: {
            std::uint8_t carry = flag_register.c;
            set_flags(flag_register, registers.D == 0, 0, 0, get_bit(registers.D, 0));
            rotate_right(registers.D);
            set_bit(registers.D, 7, carry);
            break;
        } case 0x1B: {
            std::uint8_t carry = flag_register.c;
            set_flags(flag_register, registers.E == 0, 0, 0, get_bit(registers.E, 0));
            rotate_right(registers.E);
            set_bit(registers.E, 7, carry);
            break;
        } case 0x1C: {
            std::uint8_t carry = flag_register.c;
            set_flags(flag_register, registers.H == 0, 0, 0, get_bit(registers.H, 0));
            rotate_right(registers.H);
            set_bit(registers.H, 7, carry);
            break;
        } case 0x1D: {
            std::uint8_t carry = flag_register.c;
            set_flags(flag_register, registers.L == 0, 0, 0, get_bit(registers.L, 0));
            rotate_right(registers.L);
            set_bit(registers.L, 7, carry);
            break;
        } case 0x1E: {
            std::uint8_t carry = flag_register.c;
            set_flags(flag_register, memory[HL.get()] == 0, 0, 0, get_bit(memory[HL.get()], 0));
            rotate_right(memory[HL.get()]);
            set_bit(memory[HL.get()], 7, carry);
            break;
        } case 0x1F: {
            std::uint8_t carry = flag_register.c;
            set_flags(flag_register, registers.A == 0, 0, 0, get_bit(registers.A, 0));
            rotate_right(registers.A);
            set_bit(registers.A, 7, carry);
            break;
        }
        default:
            std::cout << "Unknown opcode: " << memory[registers.program_counter] << "\n";
            break;
    }
    return;
}

void SLA(std::vector<std::uint8_t> &memory, Registers &registers, Flag_register &flag_register) {
    switch (memory[registers.program_counter + 1]) {
        case 0x20: {
            std::uint8_t carry = get_bit(registers.B, 7);
            registers.B = (std::uint8_t)(registers.B << 1);
            set_flags(flag_register, registers.B == 0, 0, 0, carry);
            break;
        } case 0x21: {
            std::uint8_t carry = get_bit(registers.C, 7);
            registers.C = (std::uint8_t)(registers.C << 1);
            set_flags(flag_register, registers.C == 0, 0, 0, carry);
            break;
        } case 0x22: {
            std::uint8_t carry = get_bit(registers.D, 7);
            registers.D = (std::uint8_t)(registers.D << 1);
            set_flags(flag_register, registers.D == 0, 0, 0, carry);
            break;
        } case 0x23: {
            std::uint8_t carry = get_bit(registers.E, 7);
            registers.E = (std::uint8_t)(registers.E << 1);
            set_flags(flag_register, registers.E == 0, 0, 0, carry);
            break;
        } case 0x24: {
            std::uint8_t carry = get_bit(registers.F, 7);
            registers.F = (std::uint8_t)(registers.F << 1);
            set_flags(flag_register, registers.F == 0, 0, 0, carry);
            break;
        } case 0x25: {
            std::uint8_t carry = get_bit(registers.H, 7);
            registers.H = (std::uint8_t)(registers.H << 1);
            set_flags(flag_register, registers.H == 0, 0, 0, carry);
            break;
        } case 0x26: {
            std::uint8_t carry = get_bit(memory[HL.get()], 7);
            memory[HL.get()] = (std::uint8_t)(memory[HL.get()] << 1);
            set_flags(flag_register, memory[HL.get()] == 0, 0, 0, carry);
            break;
        } case 0x27: {
            std::uint8_t carry = get_bit(registers.A, 7);
            registers.A = (std::uint8_t)(registers.A << 1);
            set_flags(flag_register, registers.A == 0, 0, 0, carry);
            break;
        }
        default:
            std::cout << "Unknown opcode: " << memory[registers.program_counter] << "\n";
            break;
    }
    return;
}

void SRA(std::vector<std::uint8_t> &memory, Registers &registers, Flag_register &flag_register) {
    switch (memory[registers.program_counter + 1]) {
        case 0x28: {
            std::uint8_t carry = get_bit(registers.B, 7);
            registers.B = (std::uint8_t)((registers.B >> 1) | (registers.B & 0x80));
            set_flags(flag_register, registers.B == 0, 0, 0, carry);
            break;
        } case 0x29: {
            std::uint8_t carry = get_bit(registers.C, 7);
            registers.C = (std::uint8_t)((registers.C >> 1) | (registers.C & 0x80));
            set_flags(flag_register, registers.C == 0, 0, 0, carry);
            break;
        } case 0x2A: {
            std::uint8_t carry = get_bit(registers.D, 7);
            registers.D = (std::uint8_t)((registers.D >> 1) | (registers.D & 0x80));
            set_flags(flag_register, registers.D == 0, 0, 0, carry);
            break;
        } case 0x2B: {
            std::uint8_t carry = get_bit(registers.E, 7);
            registers.E = (std::uint8_t)((registers.E >> 1) | (registers.E & 0x80));
            set_flags(flag_register, registers.E == 0, 0, 0, carry);
            break;
        } case 0x2C: {
            std::uint8_t carry = get_bit(registers.H, 7);
            registers.H = (std::uint8_t)((registers.H >> 1) | (registers.H & 0x80));
            set_flags(flag_register, registers.H == 0, 0, 0, carry);
            break;
        } case 0x2D: {
            std::uint8_t carry = get_bit(registers.L, 7);
            registers.L = (std::uint8_t)((registers.L >> 1) | (registers.L & 0x80));
            set_flags(flag_register, registers.L == 0, 0, 0, carry);
            break;
        } case 0x2E: {
            std::uint8_t carry = get_bit(memory[HL.get()], 7);
            memory[HL.get()] = (std::uint8_t)((memory[HL.get()] >> 1) | (memory[HL.get()] & 0x80));
            set_flags(flag_register, memory[HL.get()] == 0, 0, 0, carry);
            break;
        } case 0x2F: {
            std::uint8_t carry = get_bit(registers.A, 7);
            registers.A = (std::uint8_t)((registers.A >> 1) | (registers.A & 0x80));
            set_flags(flag_register, registers.A == 0, 0, 0, carry);
            break;
        }
        default:
            std::cout << "Unknown opcode: " << memory[registers.program_counter] << "\n";
            break;
    }
    return;
}

void SWAP(std::vector<std::uint8_t> &memory, Registers &registers, Flag_register &flag_register) {
    switch (memory[registers.program_counter + 1]) {
        case 0x30: {
            swap_nibbles(registers.B);
            set_flags(flag_register, registers.B == 0, 0, 0, 0);
            break;
        } case 0x31: {
            swap_nibbles(registers.C);
            set_flags(flag_register, registers.C == 0, 0, 0, 0);
            break;
        } case 0x32: {
            swap_nibbles(registers.D);
            set_flags(flag_register, registers.D == 0, 0, 0, 0);
            break;
        } case 0x33: {
            swap_nibbles(registers.E);
            set_flags(flag_register, registers.E == 0, 0, 0, 0);
            break;
        } case 0x34: {
            swap_nibbles(registers.H);
            set_flags(flag_register, registers.H == 0, 0, 0, 0);
            break;
        } case 0x35: {
            swap_nibbles(registers.L);
            set_flags(flag_register, registers.L == 0, 0, 0, 0);
            break;
        } case 0x36: {
            swap_nibbles(memory[HL.get()]);
            set_flags(flag_register, memory[HL.get()] == 0, 0, 0, 0);
            break;
        } case 0x37: {
            swap_nibbles(registers.A);
            set_flags(flag_register, registers.A == 0, 0, 0, 0);
            break;
        }
        default:
            std::cout << "Unknown opcode: " << memory[registers.program_counter] << "\n";
            break;
    }
    return;
}

void SRL(std::vector<std::uint8_t> &memory, Registers &registers, Flag_register &flag_register) {
    switch (memory[registers.program_counter + 1]) {
        case 0x38: {
            std::uint8_t carry = get_bit(registers.B, 0);
            registers.B = (std::uint8_t)(registers.B >> 1);
            set_flags(flag_register, registers.B == 0, 0, 0, carry);
            break;
        } case 0x39: {
            std::uint8_t carry = get_bit(registers.C, 0);
            registers.C = (std::uint8_t)(registers.C >> 1);
            set_flags(flag_register, registers.C == 0, 0, 0, carry);
            break;
        } case 0x3A: {
            std::uint8_t carry = get_bit(registers.D, 0);
            registers.D = (std::uint8_t)(registers.D >> 1);
            set_flags(flag_register, registers.D == 0, 0, 0, carry);
            break;
        } case 0x3B: {
            std::uint8_t carry = get_bit(registers.E, 0);
            registers.E = (std::uint8_t)(registers.E >> 1);
            set_flags(flag_register, registers.E == 0, 0, 0, carry);
            break;
        } case 0x3C: {
            std::uint8_t carry = get_bit(registers.H, 0);
            registers.H = (std::uint8_t)(registers.H >> 1);
            set_flags(flag_register, registers.H == 0, 0, 0, carry);
            break;
        } case 0x3D: {
            std::uint8_t carry = get_bit(registers.L, 0);
            registers.L = (std::uint8_t)(registers.L >> 1);
            set_flags(flag_register, registers.L == 0, 0, 0, carry);
            break;
        } case 0x3E: {
            std::uint8_t carry = get_bit(memory[HL.get()], 0);
            memory[HL.get()] = (std::uint8_t)(memory[HL.get()] >> 1);
            set_flags(flag_register, memory[HL.get()] == 0, 0, 0, carry);
            break;
        } case 0x3F: {
            std::uint8_t carry = get_bit(registers.A, 0);
            registers.A = (std::uint8_t)(registers.A >> 1);
            set_flags(flag_register, registers.A == 0, 0, 0, carry);
            break;
        }
        default:
            std::cout << "Unknown opcode: " << memory[registers.program_counter] << "\n";
            break;
    }
    return;
}

void BIT(std::vector<std::uint8_t> &memory, Registers &registers, Flag_register &flag_register) {
    switch (memory[registers.program_counter + 1]) {
        case 0x40: {
            set_flags(flag_register, get_bit(registers.B, 0) == 0, 0, 1, -1);
            break;
        } case 0x41: {
            set_flags(flag_register, get_bit(registers.C, 0) == 0, 0, 1, -1);
            break;
        } case 0x42: {
            set_flags(flag_register, get_bit(registers.D, 0) == 0, 0, 1, -1);
            break;
        } case 0x43: {
            set_flags(flag_register, get_bit(registers.E, 0) == 0, 0, 1, -1);
            break;
        } case 0x44: {
            set_flags(flag_register, get_bit(registers.H, 0) == 0, 0, 1, -1);
            break;
        } case 0x45: {
            set_flags(flag_register, get_bit(registers.L, 0) == 0, 0, 1, -1);
            break;
        } case 0x46: {
            set_flags(flag_register, get_bit(memory[HL.get()], 0) == 0, 0, 1, -1);
            break;
        } case 0x47: {
            set_flags(flag_register, get_bit(registers.A, 0) == 0, 0, 1, -1);
            break;
        } case 0x48: {
            set_flags(flag_register, get_bit(registers.B, 1) == 0, 0, 1, -1);
            break;
        } case 0x49: {
            set_flags(flag_register, get_bit(registers.C, 1) == 0, 0, 1, -1);
            break;
        } case 0x4A: {
            set_flags(flag_register, get_bit(registers.D, 1) == 0, 0, 1, -1);
            break;
        } case 0x4B: {
            set_flags(flag_register, get_bit(registers.E, 1) == 0, 0, 1, -1);
            break;
        } case 0x4C: {
            set_flags(flag_register, get_bit(registers.H, 1) == 0, 0, 1, -1);
            break;
        } case 0x4D: {
            set_flags(flag_register, get_bit(registers.L, 1) == 0, 0, 1, -1);
            break;
        } case 0x4E: {
            set_flags(flag_register, get_bit(memory[HL.get()], 1) == 0, 0, 1, -1);
            break;
        } case 0x4F: {
            set_flags(flag_register, get_bit(registers.A, 1) == 0, 0, 1, -1);
            break;
        } case 0x50: {
            set_flags(flag_register, get_bit(registers.B, 2) == 0, 0, 1, -1);
            break;
        } case 0x51: {
            set_flags(flag_register, get_bit(registers.C, 2) == 0, 0, 1, -1);
            break;
        } case 0x52: {
            set_flags(flag_register, get_bit(registers.D, 2) == 0, 0, 1, -1);
            break;
        } case 0x53: {
            set_flags(flag_register, get_bit(registers.E, 2) == 0, 0, 1, -1);
            break;
        } case 0x54: {
            set_flags(flag_register, get_bit(registers.H, 2) == 0, 0, 1, -1);
            break;
        } case 0x55: {
            set_flags(flag_register, get_bit(registers.L, 2) == 0, 0, 1, -1);
            break;
        } case 0x56: {
            set_flags(flag_register, get_bit(memory[HL.get()], 2) == 0, 0, 1, -1);
            break;
        } case 0x57: {
            set_flags(flag_register, get_bit(registers.A, 2) == 0, 0, 1, -1);
            break;
        } case 0x58: {
            set_flags(flag_register, get_bit(registers.B, 3) == 0, 0, 1, -1);
            break;
        } case 0x59: {
            set_flags(flag_register, get_bit(registers.C, 3) == 0, 0, 1, -1);
            break;
        } case 0x5A: {
            set_flags(flag_register, get_bit(registers.D, 3) == 0, 0, 1, -1);
            break;
        } case 0x5B: {
            set_flags(flag_register, get_bit(registers.E, 3) == 0, 0, 1, -1);
            break;
        } case 0x5C: {
            set_flags(flag_register, get_bit(registers.H, 3) == 0, 0, 1, -1);
            break;
        } case 0x5D: {
            set_flags(flag_register, get_bit(registers.L, 3) == 0, 0, 1, -1);
            break;
        } case 0x5E: {
            set_flags(flag_register, get_bit(memory[HL.get()], 3) == 0, 0, 1, -1);
            break;
        } case 0x5F: {
            set_flags(flag_register, get_bit(registers.A, 3) == 0, 0, 1, -1);
            break;
        } case 0x60: {
            set_flags(flag_register, get_bit(registers.B, 4) == 0, 0, 1, -1);
            break;
        } case 0x61: {
            set_flags(flag_register, get_bit(registers.C, 4) == 0, 0, 1, -1);
            break;
        } case 0x62: {
            set_flags(flag_register, get_bit(registers.D, 4) == 0, 0, 1, -1);
            break;
        } case 0x63: {
            set_flags(flag_register, get_bit(registers.E, 4) == 0, 0, 1, -1);
            break;
        } case 0x64: {
            set_flags(flag_register, get_bit(registers.H, 4) == 0, 0, 1, -1);
            break;
        } case 0x65: {
            set_flags(flag_register, get_bit(registers.L, 4) == 0, 0, 1, -1);
            break;
        } case 0x66: {
            set_flags(flag_register, get_bit(memory[HL.get()], 4) == 0, 0, 1, -1);
            break;
        } case 0x67: {
            set_flags(flag_register, get_bit(registers.A, 4) == 0, 0, 1, -1);
            break;
        } case 0x68: {
            set_flags(flag_register, get_bit(registers.B, 5) == 0, 0, 1, -1);
            break;
        } case 0x69: {
            set_flags(flag_register, get_bit(registers.C, 5) == 0, 0, 1, -1);
            break;
        } case 0x6A: {
            set_flags(flag_register, get_bit(registers.D, 5) == 0, 0, 1, -1);
            break;
        } case 0x6B: {
            set_flags(flag_register, get_bit(registers.E, 5) == 0, 0, 1, -1);
            break;
        } case 0x6C: {
            set_flags(flag_register, get_bit(registers.H, 5) == 0, 0, 1, -1);
            break;
        } case 0x6D: {
            set_flags(flag_register, get_bit(registers.L, 5) == 0, 0, 1, -1);
            break;
        } case 0x6E: {
            set_flags(flag_register, get_bit(memory[HL.get()], 5) == 0, 0, 1, -1);
            break;
        } case 0x6F: {
            set_flags(flag_register, get_bit(registers.A, 5) == 0, 0, 1, -1);
            break;
        } case 0x70: {
            set_flags(flag_register, get_bit(registers.B, 6) == 0, 0, 1, -1);
            break;
        } case 0x71: {
            set_flags(flag_register, get_bit(registers.C, 6) == 0, 0, 1, -1);
            break;
        } case 0x72: {
            set_flags(flag_register, get_bit(registers.D, 6) == 0, 0, 1, -1);
            break;
        } case 0x73: {
            set_flags(flag_register, get_bit(registers.E, 6) == 0, 0, 1, -1);
            break;
        } case 0x74: {
            set_flags(flag_register, get_bit(registers.H, 6) == 0, 0, 1, -1);
            break;
        } case 0x75: {
            set_flags(flag_register, get_bit(registers.L, 6) == 0, 0, 1, -1);
            break;
        } case 0x76: {
            set_flags(flag_register, get_bit(memory[HL.get()], 6) == 0, 0, 1, -1);
            break;
        } case 0x77: {
            set_flags(flag_register, get_bit(registers.A, 6) == 0, 0, 1, -1);
            break;
        } case 0x78: {
            set_flags(flag_register, get_bit(registers.B, 7) == 0, 0, 1, -1);
            break;
        } case 0x79: {
            set_flags(flag_register, get_bit(registers.C, 7) == 0, 0, 1, -1);
            break;
        } case 0x7A: {
            set_flags(flag_register, get_bit(registers.D, 7) == 0, 0, 1, -1);
            break;
        } case 0x7B: {
            set_flags(flag_register, get_bit(registers.E, 7) == 0, 0, 1, -1);
            break;
        } case 0x7C: {
            set_flags(flag_register, get_bit(registers.H, 7) == 0, 0, 1, -1);
            break;
        } case 0x7D: {
            set_flags(flag_register, get_bit(registers.L, 7) == 0, 0, 1, -1);
            break;
        } case 0x7E: {
            set_flags(flag_register, get_bit(memory[HL.get()], 7) == 0, 0, 1, -1);
            break;
        } case 0x7F: {
            set_flags(flag_register, get_bit(registers.A, 7) == 0, 0, 1, -1);
            break;
        }
        default:
            std::cout << "Unknown opcode: " << memory[registers.program_counter] << "\n";
            break;
    }
    return;
}

void RES(std::vector<std::uint8_t> &memory, Registers &registers, Flag_register &flag_register) {
    switch (memory[registers.program_counter + 1]) {
        case 0x80: {
            set_bit(registers.B, 0, 0);
            break;
        } case 0x81: {
            set_bit(registers.C, 0, 0);
            break;
        } case 0x82: {
            set_bit(registers.D, 0, 0);
            break;
        } case 0x83: {
            set_bit(registers.E, 0, 0);
            break;
        } case 0x84: {
            set_bit(registers.H, 0, 0);
            break;
        } case 0x85: {
            set_bit(registers.L, 0, 0);
            break;
        } case 0x86: {
            set_bit(memory[HL.get()], 0, 0);
            break;
        } case 0x87: {
            set_bit(registers.A, 0, 0);
            break;
        } case 0x88: {
            set_bit(registers.B, 1, 0);
            break;
        } case 0x89: {
            set_bit(registers.C, 1, 0);
            break;
        } case 0x8A: {
            set_bit(registers.D, 1, 0);
            break;
        } case 0x8B: {
            set_bit(registers.E, 1, 0);
            break;
        } case 0x8C: {
            set_bit(registers.H, 1, 0);
            break;
        } case 0x8D: {
            set_bit(registers.L, 1, 0);
            break;
        } case 0x8E: {
            set_bit(memory[HL.get()], 1, 0);
            break;
        } case 0x8F: {
            set_bit(registers.A, 1, 0);
            break;
        } case 0x90: {
            set_bit(registers.B, 2, 0);
            break;
        } case 0x91: {
            set_bit(registers.C, 2, 0);
            break;
        } case 0x92: {
            set_bit(registers.D, 2, 0);
            break;
        } case 0x93: {
            set_bit(registers.E, 2, 0);
            break;
        } case 0x94: {
            set_bit(registers.H, 2, 0);
            break;
        } case 0x95: {
            set_bit(registers.L, 2, 0);
            break;
        } case 0x96: {
            set_bit(memory[HL.get()], 2, 0);
            break;
        } case 0x97: {
            set_bit(registers.A, 2, 0);
            break;
        } case 0x98: {
            set_bit(registers.B, 3, 0);
            break;
        } case 0x99: {
            set_bit(registers.C, 3, 0);
            break;
        } case 0x9A: {
            set_bit(registers.D, 3, 0);
            break;
        } case 0x9B: {
            set_bit(registers.E, 3, 0);
            break;
        } case 0x9C: {
            set_bit(registers.H, 3, 0);
            break;
        } case 0x9D: {
            set_bit(registers.L, 3, 0);
            break;
        } case 0x9E: {
            set_bit(memory[HL.get()], 3, 0);
            break;
        } case 0x9F: {
            set_bit(registers.A, 3, 0);
            break;
        } case 0xA0: {
            set_bit(registers.B, 4, 0);
            break;
        } case 0xA1: {
            set_bit(registers.C, 4, 0);
            break;
        } case 0xA2: {
            set_bit(registers.D, 4, 0);
            break;
        } case 0xA3: {
            set_bit(registers.E, 4, 0);
            break;
        } case 0xA4: {
            set_bit(registers.H, 4, 0);
            break;
        } case 0xA5: {
            set_bit(registers.L, 4, 0);
            break;
        } case 0xA6: {
            set_bit(memory[HL.get()], 4, 0);
            break;
        } case 0xA7: {
            set_bit(registers.A, 4, 0);
            break;
        } case 0xA8: {
            set_bit(registers.B, 5, 0);
            break;
        } case 0xA9: {
            set_bit(registers.C, 5, 0);
            break;
        } case 0xAA: {
            set_bit(registers.D, 5, 0);
            break;
        } case 0xAB: {
            set_bit(registers.E, 5, 0);
            break;
        } case 0xAC: {
            set_bit(registers.H, 5, 0);
            break;
        } case 0xAD: {
            set_bit(registers.L, 5, 0);
            break;
        } case 0xAE: {
            set_bit(memory[HL.get()], 5, 0);
            break;
        } case 0xAF: {
            set_bit(registers.A, 5, 0);
            break;
        } case 0xB0: {
            set_bit(registers.B, 6, 0);
            break;
        } case 0xB1: {
            set_bit(registers.C, 6, 0);
            break;
        } case 0xB2: {
            set_bit(registers.D, 6, 0);
            break;
        } case 0xB3: {
            set_bit(registers.E, 6, 0);
            break;
        } case 0xB4: {
            set_bit(registers.H, 6, 0);
            break;
        } case 0xB5: {
            set_bit(registers.L, 6, 0);
            break;
        } case 0xB6: {
            set_bit(memory[HL.get()], 6, 0);
            break;
        } case 0xB7: {
            set_bit(registers.A, 6, 0);
            break;
        } case 0xB8: {
            set_bit(registers.B, 7, 0);
            break;
        } case 0xB9: {
            set_bit(registers.C, 7, 0);
            break;
        } case 0xBA: {
            set_bit(registers.D, 7, 0);
            break;
        } case 0xBB: {
            set_bit(registers.E, 7, 0);
            break;
        } case 0xBC: {
            set_bit(registers.H, 7, 0);
            break;
        } case 0xBD: {
            set_bit(registers.L, 7, 0);
            break;
        } case 0xBE: {
            set_bit(memory[HL.get()], 7, 0);
            break;
        } case 0xBF: {
            set_bit(registers.A, 7, 0);
            break;
        }
        default:
            std::cout << "Unknown opcode: " << memory[registers.program_counter] << "\n";
            break;
    }
    return;
}


void SET(std::vector<std::uint8_t> &memory, Registers &registers, Flag_register &flag_register) {
    switch (memory[registers.program_counter + 1]) {
        case 0xC0: {
            set_bit(registers.B, 0, 1);
            break;
        } case 0xC1: {
            set_bit(registers.C, 0, 1);
            break;
        } case 0xC2: {
            set_bit(registers.D, 0, 1);
            break;
        } case 0xC3: {
            set_bit(registers.E, 0, 1);
            break;
        } case 0xC4: {
            set_bit(registers.H, 0, 1);
            break;
        } case 0xC5: {
            set_bit(registers.L, 0, 1);
            break;
        } case 0xC6: {
            set_bit(memory[HL.get()], 0, 1);
            break;
        } case 0xC7: {
            set_bit(registers.A, 0, 1);
            break;
        } case 0xC8: {
            set_bit(registers.B, 1, 1);
            break;
        } case 0xC9: {
            set_bit(registers.C, 1, 1);
            break;
        } case 0xCA: {
            set_bit(registers.D, 1, 1);
            break;
        } case 0xCB: {
            set_bit(registers.E, 1, 1);
            break;
        } case 0xCC: {
            set_bit(registers.H, 1, 1);
            break;
        } case 0xCD: {
            set_bit(registers.L, 1, 1);
            break;
        } case 0xCE: {
            set_bit(memory[HL.get()], 1, 1);
            break;
        } case 0xCF: {
            set_bit(registers.A, 1, 1);
            break;
        } case 0xD0: {
            set_bit(registers.B, 2, 1);
            break;
        } case 0xD1: {
            set_bit(registers.C, 2, 1);
            break;
        } case 0xD2: {
            set_bit(registers.D, 2, 1);
            break;
        } case 0xD3: {
            set_bit(registers.E, 2, 1);
            break;
        } case 0xD4: {
            set_bit(registers.H, 2, 1);
            break;
        } case 0xD5: {
            set_bit(registers.L, 2, 1);
            break;
        } case 0xD6: {
            set_bit(memory[HL.get()], 2, 1);
            break;
        } case 0xD7: {
            set_bit(registers.A, 2, 1);
            break;
        } case 0xD8: {
            set_bit(registers.B, 3, 1);
            break;
        } case 0xD9: {
            set_bit(registers.C, 3, 1);
            break;
        } case 0xDA: {
            set_bit(registers.D, 3, 1);
            break;
        } case 0xDB: {
            set_bit(registers.E, 3, 1);
            break;
        } case 0xDC: {
            set_bit(registers.H, 3, 1);
            break;
        } case 0xDD: {
            set_bit(registers.L, 3, 1);
            break;
        } case 0xDE: {
            set_bit(memory[HL.get()], 3, 1);
            break;
        } case 0xDF: {
            set_bit(registers.A, 3, 1);
            break;
        } case 0xE0: {
            set_bit(registers.B, 4, 1);
            break;
        } case 0xE1: {
            set_bit(registers.C, 4, 1);
            break;
        } case 0xE2: {
            set_bit(registers.D, 4, 1);
            break;
        } case 0xE3: {
            set_bit(registers.E, 4, 1);
            break;
        } case 0xE4: {
            set_bit(registers.H, 4, 1);
            break;
        } case 0xE5: {
            set_bit(registers.L, 4, 1);
            break;
        } case 0xE6: {
            set_bit(memory[HL.get()], 4, 1);
            break;
        } case 0xE7: {
            set_bit(registers.A, 4, 1);
            break;
        } case 0xE8: {
            set_bit(registers.B, 5, 1);
            break;
        } case 0xE9: {
            set_bit(registers.C, 5, 1);
            break;
        } case 0xEA: {
            set_bit(registers.D, 5, 1);
            break;
        } case 0xEB: {
            set_bit(registers.E, 5, 1);
            break;
        } case 0xEC: {
            set_bit(registers.H, 5, 1);
            break;
        } case 0xED: {
            set_bit(registers.L, 5, 1);
            break;
        } case 0xEE: {
            set_bit(memory[HL.get()], 5, 1);
            break;
        } case 0xEF: {
            set_bit(registers.A, 5, 1);
            break;
        } case 0xF0: {
            set_bit(registers.B, 6, 1);
            break;
        } case 0xF1: {
            set_bit(registers.C, 6, 1);
            break;
        } case 0xF2: {
            set_bit(registers.D, 6, 1);
            break;
        } case 0xF3: {
            set_bit(registers.E, 6, 1);
            break;
        } case 0xF4: {
            set_bit(registers.H, 6, 1);
            break;
        } case 0xF5: {
            set_bit(registers.L, 6, 1);
            break;
        } case 0xF6: {
            set_bit(memory[HL.get()], 6, 1);
            break;
        } case 0xF7: {
            set_bit(registers.A, 6, 1);
            break;
        } case 0xF8: {
            set_bit(registers.B, 7, 1);
            break;
        } case 0xF9: {
            set_bit(registers.C, 7, 1);
            break;
        } case 0xFA: {
            set_bit(registers.D, 7, 1);
            break;
        } case 0xFB: {
            set_bit(registers.E, 7, 1);
            break;
        } case 0xFC: {
            set_bit(registers.H, 7, 1);
            break;
        } case 0xFD: {
            set_bit(registers.L, 7, 1);
            break;
        } case 0xFE: {
            set_bit(memory[HL.get()], 7, 1);
            break;
        } case 0xFF: {
            set_bit(registers.A, 7, 1);
            break;
        }
        default:
            std::cout << "Unknown opcode: " << memory[registers.program_counter] << "\n";
            break;
    }
    return;
}


void cpu_cycle(std::vector<std::uint8_t> &memory, Registers &registers, Flag_register &flag_register) {

    switch (memory[registers.program_counter] & 0xF0) {
        case 0x00: {
            break;
        } case 0x80: {
            registers.A = registers.A + registers.B;
            break;
        } case 0xC0: {
            if (memory[registers.program_counter] == 0xCB) {
                switch (memory[registers.program_counter + 1] & 0xF0) {
                    case 0x00: {
                        break;
                    } case 0x80: {
                        registers.A = registers.A + registers.B;
                        break;
                    } case 0xC0: {
                        if (memory[registers.program_counter] == 0xCB) {

                        }
                        break;
                    }
                    default:
                        std::cout << "Unknown opcode: " << memory[registers.program_counter] << "\n";
                        break;
                }
            }
            break;
        }
        default:
            std::cout << "Unknown opcode: " << memory[registers.program_counter] << "\n";
            break;
    }
    registers.program_counter++;
    return;
}