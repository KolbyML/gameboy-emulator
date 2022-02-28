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

void cpu_cycle(std::vector<std::uint8_t> &memory, Registers &registers, Flag_register &flag_register) {

    switch (memory[registers.program_counter] & 0xF0) {
        case 0x00: {
            break;
        } case 0x80: {
            registers.A = registers.A + registers.B;
            break;
        }
        default:
            std::cout << "Unknown opcode: " << memory[registers.program_counter] << "\n";
            break;
    }
    registers.program_counter++;
    return;
}