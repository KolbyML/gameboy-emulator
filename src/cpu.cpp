//
// Created by kolby on 2/20/2022.
//

#include <cstdint>
#include <vector>
#include <iostream>
#include <iomanip>
#include "cpu.h"
#include "utils.h"

uint8_t op_code = 0;

Registers registers = {};
Virtual_Register AF = Virtual_Register(registers.A, registers.F);
Virtual_Register BC = Virtual_Register(registers.B, registers.C);
Virtual_Register DE = Virtual_Register(registers.D, registers.E);
Virtual_Register HL = Virtual_Register(registers.H, registers.L);
bool IME = false;

void set_flags(Flag_register &flag_register, std::int8_t Z, std::int8_t N, std::int8_t H, std::int8_t C) {
    if (Z != -1) flag_register.z = Z;
    if (N != -1) flag_register.n = N;
    if (H != -1) flag_register.h = H;
    if (C != -1) flag_register.c = C;
}

void INC(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    switch (op_code) {
        case 0x03: {
            BC.increment(1);
            break;
        } case 0x13: {
            DE.increment(1);
            break;
        } case 0x23: {
            HL.increment(1);
            break;
        } case 0x33: {
            registers.stack_pointer += 1;
            break;
        } case 0x04: {
            set_flags(flag_register, (uint8_t)(registers.B + 1) == 0, 0, is_half_carry_u8(registers.B, 1), -1);
            registers.B += 1;
            break;
        } case 0x14: {
            set_flags(flag_register, (uint8_t)(registers.D + 1) == 0, 0, is_half_carry_u8(registers.D, 1), -1);
            registers.D += 1;
            break;
        } case 0x24: {
            set_flags(flag_register, (uint8_t)(registers.H + 1) == 0, 0, is_half_carry_u8(registers.H, 1), -1);
            registers.H += 1;
            break;
        } case 0x34: {
            set_flags(flag_register, (uint8_t)(memory[HL.get()] + 1) == 0, 0, is_half_carry_u8(memory[HL.get()], 1), -1);
            memory[HL.get()] += 1;
            break;
        } case 0x0C: {
            set_flags(flag_register, (uint8_t)(registers.C + 1) == 0, 0, is_half_carry_u8(registers.C, 1), -1);
            registers.C += 1;
            break;
        } case 0x1C: {
            set_flags(flag_register, (uint8_t)(registers.E + 1) == 0, 0, is_half_carry_u8(registers.E, 1), -1);
            registers.E += 1;
            break;
        } case 0x2C: {
            set_flags(flag_register, (uint8_t)(registers.L + 1) == 0, 0, is_half_carry_u8(registers.L, 1), -1);
            registers.L += 1;
            break;
        } case 0x3C: {
            set_flags(flag_register, (uint8_t)(registers.A + 1) == 0, 0, is_half_carry_u8(registers.A, 1), -1);
            registers.A += 1;
            break;
        }
        default:
            std::cout << __FUNCTION__ << " Unknown opcode: " << (int)op_code << "\n";
            break;
    }
    return;
}

void DEC(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    switch (op_code) {
        case 0x05: {
            set_flags(flag_register, (uint8_t)(registers.B - 1) == 0, 1, is_borrow_from_bit4(registers.B, 1), -1);
            registers.B -= 1;
            break;
        } case 0x15: {
            set_flags(flag_register, (uint8_t)(registers.D - 1) == 0, 1, is_borrow_from_bit4(registers.D, 1), -1);
            registers.D -= 1;
            break;
        } case 0x25: {
            set_flags(flag_register, (uint8_t)(registers.H - 1) == 0, 1, is_borrow_from_bit4(registers.H, 1), -1);
            registers.H -= 1;
            break;
        } case 0x35: {
            set_flags(flag_register, (uint8_t)(memory[HL.get()] - 1) == 0, 1, is_borrow_from_bit4(memory[HL.get()], 1), -1);
            memory[HL.get()] -= 1;
            break;
        } case 0x0B: {
            BC.decrement(1);
            break;
        } case 0x1B: {
            DE.decrement(1);
            break;
        } case 0x2B: {
            HL.decrement(1);
            break;
        } case 0x3B: {
            registers.stack_pointer -= 1;
            break;
        } case 0x0D: {
            set_flags(flag_register, (uint8_t)(registers.C - 1) == 0, 1, is_borrow_from_bit4(registers.C, 1), -1);
            registers.C -= 1;
            break;
        } case 0x1D: {
            set_flags(flag_register, (uint8_t)(registers.E - 1) == 0, 1, is_borrow_from_bit4(registers.E, 1), -1);
            registers.E -= 1;
            break;
        } case 0x2D: {
            set_flags(flag_register, (uint8_t)(registers.L - 1) == 0, 1, is_borrow_from_bit4(registers.L, 1), -1);
            registers.L -= 1;
            break;
        } case 0x3D: {
            set_flags(flag_register, (uint8_t)(registers.A - 1) == 0, 1, is_borrow_from_bit4(registers.A, 1), -1);
            registers.A -= 1;
            break;
        }
        default:
            std::cout << __FUNCTION__ << " Unknown opcode: " << (int)op_code << "\n";
            break;
    }
    return;
}

void CALL(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    switch (op_code) {
        case 0xC4: {
            if (!(bool)flag_register.z) {
                registers.stack_pointer -= 1;
                memory[registers.stack_pointer] = ((registers.program_counter + 2) & 0xFF00) >> 8;
                registers.stack_pointer -= 1;
                memory[registers.stack_pointer] = ((registers.program_counter + 2) & 0x00FF);
                uint16_t nnnn = Virtual_Register(memory[registers.program_counter + 1], memory[registers.program_counter]).get();
                registers.program_counter = nnnn;
            } else registers.program_counter += 2;
            break;
        } case 0xD4: {
            if (!(bool)flag_register.c) {
                registers.stack_pointer -= 1;
                memory[registers.stack_pointer] = ((registers.program_counter + 2) & 0xFF00) >> 8;
                registers.stack_pointer -= 1;
                memory[registers.stack_pointer] = ((registers.program_counter + 2) & 0x00FF);
                uint16_t nnnn = Virtual_Register(memory[registers.program_counter + 1], memory[registers.program_counter]).get();
                registers.program_counter = nnnn;
            } else registers.program_counter += 2;
            break;
        } case 0xCC: {
            if ((bool)flag_register.z) {
                registers.stack_pointer -= 1;
                memory[registers.stack_pointer] = ((registers.program_counter + 2) & 0xFF00) >> 8;
                registers.stack_pointer -= 1;
                memory[registers.stack_pointer] = ((registers.program_counter + 2) & 0x00FF);
                uint16_t nnnn = Virtual_Register(memory[registers.program_counter + 1], memory[registers.program_counter]).get();
                registers.program_counter = nnnn;
            } else registers.program_counter += 2;
            break;
        } case 0xDC: {
            if ((bool)flag_register.c) {
                registers.stack_pointer -= 1;
                memory[registers.stack_pointer] = ((registers.program_counter + 2) & 0xFF00) >> 8;
                registers.stack_pointer -= 1;
                memory[registers.stack_pointer] = ((registers.program_counter + 2) & 0x00FF);
                uint16_t nnnn = Virtual_Register(memory[registers.program_counter + 1], memory[registers.program_counter]).get();
                registers.program_counter = nnnn;
            } else registers.program_counter += 2;
            break;
        } case 0xCD: {
            registers.stack_pointer -= 1;
            memory[registers.stack_pointer] = ((registers.program_counter + 2) & 0xFF00) >> 8;
            registers.stack_pointer -= 1;
            memory[registers.stack_pointer] = ((registers.program_counter + 2) & 0x00FF);
            uint16_t nnnn = Virtual_Register(memory[registers.program_counter + 1], memory[registers.program_counter]).get();
            registers.program_counter = nnnn;
            break;
        }
        default:
            std::cout << __FUNCTION__ << " Unknown opcode: " << (int)op_code << "\n";
            break;
    }
    return;
}

void RST(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    switch (op_code) {
        case 0xC7: {
            registers.stack_pointer -= 1;
            memory[registers.stack_pointer] = ((registers.program_counter) & 0xFF00) >> 8;
            registers.stack_pointer -= 1;
            memory[registers.stack_pointer] = ((registers.program_counter) & 0x00FF);
            uint16_t nnnn = 0x00;
            registers.program_counter = nnnn;
            break;
        } case 0xD7: {
            registers.stack_pointer -= 1;
            memory[registers.stack_pointer] = ((registers.program_counter) & 0xFF00) >> 8;
            registers.stack_pointer -= 1;
            memory[registers.stack_pointer] = ((registers.program_counter) & 0x00FF);
            uint16_t nnnn = 0x10;
            registers.program_counter = nnnn;
            break;
        } case 0xE7: {
            registers.stack_pointer -= 1;
            memory[registers.stack_pointer] = ((registers.program_counter) & 0xFF00) >> 8;
            registers.stack_pointer -= 1;
            memory[registers.stack_pointer] = ((registers.program_counter) & 0x00FF);
            uint16_t nnnn = 0x20;
            registers.program_counter = nnnn;
            break;
        } case 0xF7: {
            registers.stack_pointer -= 1;
            memory[registers.stack_pointer] = ((registers.program_counter) & 0xFF00) >> 8;
            registers.stack_pointer -= 1;
            memory[registers.stack_pointer] = ((registers.program_counter) & 0x00FF);
            uint16_t nnnn = 0x30;
            registers.program_counter = nnnn;
            break;
        } case 0xCF: {
            registers.stack_pointer -= 1;
            memory[registers.stack_pointer] = ((registers.program_counter) & 0xFF00) >> 8;
            registers.stack_pointer -= 1;
            memory[registers.stack_pointer] = ((registers.program_counter) & 0x00FF);
            uint16_t nnnn = 0x08;
            registers.program_counter = nnnn;
            break;
        } case 0xDF: {
            registers.stack_pointer -= 1;
            memory[registers.stack_pointer] = ((registers.program_counter) & 0xFF00) >> 8;
            registers.stack_pointer -= 1;
            memory[registers.stack_pointer] = ((registers.program_counter) & 0x00FF);
            uint16_t nnnn = 0x18;
            registers.program_counter = nnnn;
            break;
        } case 0xEF: {
            registers.stack_pointer -= 1;
            memory[registers.stack_pointer] = ((registers.program_counter) & 0xFF00) >> 8;
            registers.stack_pointer -= 1;
            memory[registers.stack_pointer] = ((registers.program_counter) & 0x00FF);
            uint16_t nnnn = 0x28;
            registers.program_counter = nnnn;
            break;
        } case 0xFF: {
            registers.stack_pointer -= 1;
            memory[registers.stack_pointer] = ((registers.program_counter) & 0xFF00) >> 8;
            registers.stack_pointer -= 1;
            memory[registers.stack_pointer] = ((registers.program_counter) & 0x00FF);
            uint16_t nnnn = 0x38;
            registers.program_counter = nnnn;
            break;
        }
        default:
            std::cout << __FUNCTION__ << " Unknown opcode: " << (int)op_code << "\n";
            break;
    }
    return;
}

void JP(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    switch (op_code) {
        case 0xC2: {
            if (!(bool)flag_register.z) {
                uint16_t nnnn = Virtual_Register(memory[registers.program_counter + 1], memory[registers.program_counter]).get();
                registers.program_counter = nnnn;
            } else registers.program_counter += 2;
            break;
        } case 0xD2: {
            if (!(bool)flag_register.c) {
                uint16_t nnnn = Virtual_Register(memory[registers.program_counter + 1], memory[registers.program_counter]).get();
                registers.program_counter = nnnn;
            } else registers.program_counter += 2;
            break;
        } case 0xC3: {
            uint16_t nnnn = Virtual_Register(memory[registers.program_counter + 1], memory[registers.program_counter]).get();
            registers.program_counter = nnnn;
            break;
        } case 0xE9: {
            registers.program_counter = HL.get();
            break;
        } case 0xCA: {
            if ((bool)flag_register.z) {
                uint16_t nnnn = Virtual_Register(memory[registers.program_counter + 1], memory[registers.program_counter]).get();
                registers.program_counter = nnnn;
            } else registers.program_counter += 2;
            break;
        } case 0xDA: {
            if ((bool)flag_register.c) {
                uint16_t nnnn = Virtual_Register(memory[registers.program_counter + 1], memory[registers.program_counter]).get();
                registers.program_counter = nnnn;
            } else registers.program_counter += 2;
            break;
        }
        default:
            std::cout << __FUNCTION__ << " Unknown opcode: " << (int)op_code << "\n";
            break;
    }
    return;
}

void JR(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    switch (op_code) {
        case 0x20: {
            if (!(bool)flag_register.z) {
                int8_t nn = memory[registers.program_counter];
                registers.program_counter += nn;
            }
            break;
        } case 0x30: {
            if (!(bool)flag_register.c) {
                int8_t nn = memory[registers.program_counter];
                registers.program_counter += nn;
            }
            break;
        } case 0x18: {
            std::int8_t nn = memory[registers.program_counter];
            registers.program_counter += nn;
            break;
        } case 0x28: {
            if ((bool)flag_register.z) {
                std::int8_t nn = memory[registers.program_counter];
                registers.program_counter += nn;
            }
            break;
        } case 0x38: {
            if ((bool)flag_register.c) {
                std::int8_t nn = memory[registers.program_counter];
                registers.program_counter += nn;
            }
            break;
        }
        default:
            std::cout << __FUNCTION__ << " Unknown opcode: " << (int)op_code << "\n";
            break;
    }
    registers.program_counter += 1;
    return;
}

void POP(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    switch (op_code) {
        case 0xC1: {
            registers.C = memory[registers.stack_pointer];
            registers.stack_pointer += 1;
            registers.B = memory[registers.stack_pointer];
            registers.stack_pointer += 1;
            break;
        } case 0xD1: {
            registers.E = memory[registers.stack_pointer];
            registers.stack_pointer += 1;
            registers.D = memory[registers.stack_pointer];
            registers.stack_pointer += 1;
            break;
        } case 0xE1: {
            registers.L = memory[registers.stack_pointer];
            registers.stack_pointer += 1;
            registers.H = memory[registers.stack_pointer];
            registers.stack_pointer += 1;
            break;
        } case 0xF1: {
            uint8_t reg_F = memory[registers.stack_pointer];
            set_flags(flag_register, get_bit(reg_F, 7), get_bit(reg_F, 6), get_bit(reg_F, 5), get_bit(reg_F, 4));
            registers.stack_pointer += 1;
            registers.A = memory[registers.stack_pointer];
            registers.stack_pointer += 1;
            break;
        }
        default:
            std::cout << __FUNCTION__ << " Unknown opcode: " << (int)op_code << "\n";
            break;
    }
    return;
}

void PUSH(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    switch (op_code) {
        case 0xC5: {
            registers.stack_pointer -= 1;
            memory[registers.stack_pointer] = registers.B;
            registers.stack_pointer -= 1;
            memory[registers.stack_pointer] = registers.C;
            break;
        } case 0xD5: {
            registers.stack_pointer -= 1;
            memory[registers.stack_pointer] = registers.D;
            registers.stack_pointer -= 1;
            memory[registers.stack_pointer] = registers.E;
            break;
        } case 0xE5: {
            registers.stack_pointer -= 1;
            memory[registers.stack_pointer] = registers.H;
            registers.stack_pointer -= 1;
            memory[registers.stack_pointer] = registers.L;
            break;
        } case 0xF5: {
            registers.stack_pointer -= 1;
            memory[registers.stack_pointer] = registers.A;
            registers.stack_pointer -= 1;
            memory[registers.stack_pointer] = (uint8_t)(flag_register.z << 7 | flag_register.n << 6 | flag_register.h << 5 | flag_register.c << 4);
            break;
        }
        default:
            std::cout << __FUNCTION__ << " Unknown opcode: " << (int)op_code << "\n";
            break;
    }
    return;
}

void RET(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    switch (op_code) {
        case 0xC0: {
            if (!(bool)flag_register.z) {
                registers.program_counter = Virtual_Register(memory[registers.stack_pointer + 1], memory[registers.stack_pointer]).get();
                registers.stack_pointer += 2;
            }
            break;
        } case 0xD0: {
            if (!(bool)flag_register.c) {
                registers.program_counter = Virtual_Register(memory[registers.stack_pointer + 1], memory[registers.stack_pointer]).get();
                registers.stack_pointer += 2;
            }
            break;
        } case 0xC9: {
            registers.program_counter = Virtual_Register(memory[registers.stack_pointer + 1], memory[registers.stack_pointer]).get();
            registers.stack_pointer += 2;
            break;
        } case 0xC8: {
            if ((bool)flag_register.z) {
                registers.program_counter = Virtual_Register(memory[registers.stack_pointer + 1], memory[registers.stack_pointer]).get();
                registers.stack_pointer += 2;
            }
            break;
        } case 0xD8: {
            if ((bool)flag_register.c) {
                registers.program_counter = Virtual_Register(memory[registers.stack_pointer + 1], memory[registers.stack_pointer]).get();
                registers.stack_pointer += 2;
            }
            break;
        }
        default:
            std::cout << __FUNCTION__ << " Unknown opcode: " << (int)op_code << "\n";
            break;
    }
    return;
}

void RLCA(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    set_flags(flag_register, 0, 0, 0, get_bit(registers.A, 7));
    rotate_left(registers.A);
    return;
}

void RLA(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    uint8_t carry = flag_register.c;
    set_flags(flag_register, 0, 0, 0, get_bit(registers.A, 7));
    rotate_left(registers.A);
    set_bit(registers.A, 0, carry);
    return;
}

void RRCA(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    set_flags(flag_register, 0, 0, 0, get_bit(registers.A, 0));
    rotate_right(registers.A);
    return;
}

void RRA(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    uint8_t carry = flag_register.c;
    set_flags(flag_register, 0, 0, 0, get_bit(registers.A, 0));
    rotate_right(registers.A);
    set_bit(registers.A, 7, carry);
    return;
}

void DAA(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    if (!flag_register.n) {
        if (flag_register.c || registers.A > 0x99) { registers.A += 0x60; flag_register.c = 1; }
        if (flag_register.h || (registers.A & 0x0f) > 0x09) { registers.A += 0x6; }
    } else {
        if (flag_register.c) { registers.A -= 0x60; }
        if (flag_register.h) { registers.A -= 0x6; }
    }

    flag_register.z = (registers.A == 0);
    flag_register.h = 0;
    return;
}

void SCF(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    set_flags(flag_register, -1, 0, 0, 1);
    return;
}

void CPL(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    set_flags(flag_register, -1, 1, 1, -1);
    registers.A = ~registers.A;
    return;
}

void CCF(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    set_flags(flag_register, -1, 0, 0, flag_register.c ^ 1);
    return;
}

void DI(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    IME = false;
    return;
}

void EI(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    IME = true;
    return;
}

void RETI(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    IME = true;
    registers.program_counter = Virtual_Register(memory[registers.stack_pointer + 1], memory[registers.stack_pointer]).get();
    registers.stack_pointer += 2;
    return;
}

void NOP(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    return;
}

void HALT(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    if (IME) {

    } else {

    }
    return;
}

void SUB(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    switch (op_code) {
        case 0x90: {
            set_flags(flag_register, (uint8_t)(registers.A - registers.B) == 0, 1, is_borrow_from_bit4(registers.A, registers.B), registers.B > registers.A);
            registers.A = registers.A - registers.B;
            break;
        } case 0x91: {
            set_flags(flag_register, (uint8_t)(registers.A - registers.C) == 0, 1, is_borrow_from_bit4(registers.A, registers.C), registers.C > registers.A);
            registers.A = registers.A - registers.C;
            break;
        } case 0x92: {
            set_flags(flag_register, (uint8_t)(registers.A - registers.D) == 0, 1, is_borrow_from_bit4(registers.A, registers.D), registers.D > registers.A);
            registers.A = registers.A - registers.D;
            break;
        } case 0x93: {
            set_flags(flag_register, (uint8_t)(registers.A - registers.E) == 0, 1, is_borrow_from_bit4(registers.A, registers.E), registers.E > registers.A);
            registers.A = registers.A - registers.E;
            break;
        } case 0x94: {
            set_flags(flag_register, (uint8_t)(registers.A - registers.H) == 0, 1, is_borrow_from_bit4(registers.A, registers.H), registers.H > registers.A);
            registers.A = registers.A - registers.H;
            break;
        } case 0x95: {
            set_flags(flag_register, (uint8_t)(registers.A - registers.L) == 0, 1, is_borrow_from_bit4(registers.A, registers.L), registers.L > registers.A);
            registers.A = registers.A - registers.L;
            break;
        } case 0x96: {
            set_flags(flag_register, (uint8_t)(registers.A - memory[HL.get()]) == 0, 1, is_borrow_from_bit4(registers.A, memory[HL.get()]), memory[HL.get()] > registers.A);
            registers.A = registers.A - memory[HL.get()];
            break;
        } case 0x97: {
            set_flags(flag_register, (uint8_t)(registers.A - registers.A) == 0, 1, is_borrow_from_bit4(registers.A, registers.A), 0);
            registers.A = registers.A - registers.A;
            break;
        } case 0xD6: {
            uint8_t nn = memory[registers.program_counter];
            set_flags(flag_register, (uint8_t)(registers.A - nn) == 0, 1, is_borrow_from_bit4(registers.A, nn), nn > registers.A);
            registers.A = registers.A - nn;
            registers.program_counter += 1;
            break;
        }
        default:
            std::cout << __FUNCTION__ << " Unknown opcode: " << (int)op_code << "\n";
            break;
    }
    return;
}

void SBC(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    switch (op_code) {
        case 0x98: {
            uint8_t c = flag_register.c;
            set_flags(flag_register, (uint8_t)(registers.A - registers.B - c) == 0, 1, is_borrow_from_bit4(registers.A, registers.B, c), registers.B + c > registers.A);
            registers.A = registers.A - registers.B - c;
            break;
        } case 0x99: {
            uint8_t c = flag_register.c;
            set_flags(flag_register, (uint8_t)(registers.A - registers.C - c) == 0, 1, is_borrow_from_bit4(registers.A, registers.C, c), registers.C + c > registers.A);
            registers.A = registers.A - registers.C - c;
            break;
        } case 0x9A: {
            uint8_t c = flag_register.c;
            set_flags(flag_register, (uint8_t)(registers.A - registers.D - c) == 0, 1, is_borrow_from_bit4(registers.A, registers.D, c), registers.D + c > registers.A);
            registers.A = registers.A - registers.D - c;
            break;
        } case 0x9B: {
            uint8_t c = flag_register.c;
            set_flags(flag_register, (uint8_t)(registers.A - registers.E - c) == 0, 1, is_borrow_from_bit4(registers.A, registers.E, c), registers.E + c > registers.A);
            registers.A = registers.A - registers.E - c;
            break;
        } case 0x9C: {
            uint8_t c = flag_register.c;
            set_flags(flag_register, (uint8_t)(registers.A - registers.H - c) == 0, 1, is_borrow_from_bit4(registers.A, registers.H, c), registers.H + c > registers.A);
            registers.A = registers.A - registers.H - c;
            break;
        } case 0x9D: {
            uint8_t c = flag_register.c;
            set_flags(flag_register, (uint8_t)(registers.A - registers.L - c) == 0, 1, is_borrow_from_bit4(registers.A, registers.L, c), registers.L + c > registers.A);
            registers.A = registers.A - registers.L - c;
            break;
        } case 0x9E: {
            uint8_t c = flag_register.c;
            set_flags(flag_register, (uint8_t)(registers.A - memory[HL.get()] - c) == 0, 1, is_borrow_from_bit4(registers.A, memory[HL.get()], c), memory[HL.get()] + c > registers.A);
            registers.A = registers.A - memory[HL.get()] - c;
            break;
        } case 0x9F: {
            uint8_t c = flag_register.c;
            set_flags(flag_register, (uint8_t)(registers.A - registers.A - c) == 0, 1, is_borrow_from_bit4(registers.A, registers.A, c), registers.A + c > registers.A);
            registers.A = registers.A - registers.A - c;
            break;
        } case 0xDE: {
            uint8_t c = flag_register.c;
            uint8_t nn = memory[registers.program_counter];
            set_flags(flag_register, (uint8_t)(registers.A - nn - c) == 0, 1, is_borrow_from_bit4(registers.A, nn, c), nn + c > registers.A);
            registers.A = registers.A - nn - c;
            registers.program_counter += 1;
            break;
        }
        default:
            std::cout << __FUNCTION__ << " Unknown opcode: " << (int)op_code << "\n";
            break;
    }
    return;
}

void CP(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    switch (op_code) {
        case 0xB8: {
            set_flags(flag_register, (uint8_t)(registers.A - registers.B) == 0, 1, is_borrow_from_bit4(registers.A, registers.B), registers.B > registers.A);
            break;
        } case 0xB9: {
            set_flags(flag_register, (uint8_t)(registers.A - registers.C) == 0, 1, is_borrow_from_bit4(registers.A, registers.C), registers.C > registers.A);
            break;
        } case 0xBA: {
            set_flags(flag_register, (uint8_t)(registers.A - registers.D) == 0, 1, is_borrow_from_bit4(registers.A, registers.D), registers.D > registers.A);
            break;
        } case 0xBB: {
            set_flags(flag_register, (uint8_t)(registers.A - registers.E) == 0, 1, is_borrow_from_bit4(registers.A, registers.E), registers.E > registers.A);
            break;
        } case 0xBC: {
            set_flags(flag_register, (uint8_t)(registers.A - registers.H) == 0, 1, is_borrow_from_bit4(registers.A, registers.H), registers.H > registers.A);
            break;
        } case 0xBD: {
            set_flags(flag_register, (uint8_t)(registers.A - registers.L) == 0, 1, is_borrow_from_bit4(registers.A, registers.L), registers.L > registers.A);
            break;
        } case 0xBE: {
            set_flags(flag_register, (uint8_t)(registers.A - memory[HL.get()]) == 0, 1, ((registers.A & 0xF) < (memory[HL.get()] & 0xF)), memory[HL.get()] > registers.A);
            break;
        } case 0xBF: {
            set_flags(flag_register, (uint8_t)(registers.A - registers.A) == 0, 1, is_borrow_from_bit4(registers.A, registers.A), 0);
            break;
        } case 0xFE: {
            uint8_t nn = memory[registers.program_counter];
            set_flags(flag_register, (uint8_t)(registers.A - nn) == 0, 1, is_borrow_from_bit4(registers.A, nn), nn > registers.A);
            registers.program_counter += 1;
            break;
        }
        default:
            std::cout << __FUNCTION__ << " Unknown opcode: " << (int)op_code << "\n";
            break;
    }
    return;
}

void ADD(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    switch (op_code) {
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
            set_flags(flag_register, (uint8_t)(registers.A + registers.B) == 0, 0, is_half_carry_u8(registers.A, registers.B), is_carry_u8(registers.A, registers.B));
            registers.A = registers.A + registers.B;
            break;
        } case 0x81: {
            set_flags(flag_register, (uint8_t)(registers.A + registers.C) == 0, 0, is_half_carry_u8(registers.A, registers.C), is_carry_u8(registers.A, registers.C));
            registers.A = registers.A + registers.C;
            break;
        } case 0x82: {
            set_flags(flag_register, (uint8_t)(registers.A + registers.D) == 0, 0, is_half_carry_u8(registers.A, registers.D), is_carry_u8(registers.A, registers.D));
            registers.A = registers.A + registers.D;
            break;
        } case 0x83: {
            set_flags(flag_register, (uint8_t)(registers.A + registers.E) == 0, 0, is_half_carry_u8(registers.A, registers.E), is_carry_u8(registers.A, registers.E));
            registers.A = registers.A + registers.E;
            break;
        } case 0x84: {
            set_flags(flag_register, (uint8_t)(registers.A + registers.H) == 0, 0, is_half_carry_u8(registers.A, registers.H), is_carry_u8(registers.A, registers.H));
            registers.A = registers.A + registers.H;
            break;
        } case 0x85: {
            set_flags(flag_register, (uint8_t)(registers.A + registers.L) == 0, 0, is_half_carry_u8(registers.A, registers.L), is_carry_u8(registers.A, registers.L));
            registers.A = registers.A + registers.L;
            break;
        } case 0x86: {
            set_flags(flag_register, (uint8_t)(registers.A + memory[HL.get()]) == 0, 0, is_half_carry_u8(registers.A, memory[HL.get()]), is_carry_u8_u16(registers.A, memory[HL.get()]));
            registers.A = registers.A + memory[HL.get()];
            break;
        } case 0x87: {
            set_flags(flag_register, (uint8_t)(registers.A + registers.A) == 0, 0, is_half_carry_u8(registers.A, registers.A), is_carry_u8(registers.A, registers.A));
            registers.A = registers.A + registers.A;
            break;
        } case 0xC6: {
            uint8_t nn = memory[registers.program_counter];
            set_flags(flag_register, (uint8_t)(registers.A + nn) == 0, 0, is_half_carry_u8(registers.A, nn), is_carry_u8(registers.A, nn));
            registers.A = registers.A + nn;
            registers.program_counter += 1;
            break;
        } case 0xE8: {
            std::int8_t nn = memory[registers.program_counter];
            set_flags(flag_register, 0, 0, (((registers.stack_pointer & 0xF) + (nn & 0xF)) > 0x0F), (((registers.stack_pointer & 0xFF) + (nn & 0xFF)) > 0xFF));
            registers.stack_pointer = registers.stack_pointer + nn;
            registers.program_counter += 1;
            break;
        }
        default:
            std::cout << __FUNCTION__ << " Unknown opcode: " << (int)op_code << "\n";
            break;
    }
    return;
}

void AND(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    switch (op_code) {
        case 0xA0: {
            registers.A = registers.A & registers.B;
            set_flags(flag_register, registers.A == 0, 0, 1, 0);
            break;
        } case 0xA1: {
            registers.A = registers.A & registers.C;
            set_flags(flag_register, registers.A == 0, 0, 1, 0);
            break;
        } case 0xA2: {
            registers.A = registers.A & registers.D;
            set_flags(flag_register, registers.A == 0, 0, 1, 0);
            break;
        } case 0xA3: {
            registers.A = registers.A & registers.E;
            set_flags(flag_register, registers.A == 0, 0, 1, 0);
            break;
        } case 0xA4: {
            registers.A = registers.A & registers.H;
            set_flags(flag_register, registers.A == 0, 0, 1, 0);
            break;
        } case 0xA5: {
            registers.A = registers.A & registers.L;
            set_flags(flag_register, registers.A == 0, 0, 1, 0);
            break;
        } case 0xA6: {
            registers.A = registers.A & memory[HL.get()];
            set_flags(flag_register, registers.A == 0, 0, 1, 0);
            break;
        } case 0xA7: {
            registers.A = registers.A & registers.A;
            set_flags(flag_register, registers.A == 0, 0, 1, 0);
            break;
        } case 0xE6: {
            uint8_t nn = memory[registers.program_counter];
            registers.A = registers.A & nn;
            set_flags(flag_register, registers.A == 0, 0, 1, 0);
            registers.program_counter += 1;
            break;
        }
        default:
            std::cout << __FUNCTION__ << " Unknown opcode: " << (int)op_code << "\n";
            return;
    }
    return;
}

void XOR(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    switch (op_code) {
        case 0xA8: {
            registers.A = registers.A ^ registers.B;
            set_flags(flag_register, registers.A == 0, 0, 0, 0);
            break;
        } case 0xA9: {
            registers.A = registers.A ^ registers.C;
            set_flags(flag_register, registers.A == 0, 0, 0, 0);
            break;
        } case 0xAA: {
            registers.A = registers.A ^ registers.D;
            set_flags(flag_register, registers.A == 0, 0, 0, 0);
            break;
        } case 0xAB: {
            registers.A = registers.A ^ registers.E;
            set_flags(flag_register, registers.A == 0, 0, 0, 0);
            break;
        } case 0xAC: {
            registers.A = registers.A ^ registers.H;
            set_flags(flag_register, registers.A == 0, 0, 0, 0);
            break;
        } case 0xAD: {
            registers.A = registers.A ^ registers.L;
            set_flags(flag_register, registers.A == 0, 0, 0, 0);
            break;
        } case 0xAE: {
            registers.A = registers.A ^ memory[HL.get()];
            set_flags(flag_register, registers.A == 0, 0, 0, 0);
            break;
        } case 0xAF: {
            registers.A = registers.A ^ registers.A;
            set_flags(flag_register, registers.A == 0, 0, 0, 0);
            break;
        } case 0xEE: {
            uint8_t nn = memory[registers.program_counter];
            registers.A = registers.A ^ nn;
            set_flags(flag_register, registers.A == 0, 0, 0, 0);
            registers.program_counter += 1;
            break;
        }
        default:
            std::cout << __FUNCTION__ << " Unknown opcode: " << (int)op_code << "\n";
            return;
    }
    return;
}

void OR(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    switch (op_code) {
        case 0xB0: {
            registers.A = registers.A | registers.B;
            set_flags(flag_register, registers.A == 0, 0, 0, 0);
            break;
        } case 0xB1: {
            registers.A = registers.A | registers.C;
            set_flags(flag_register, registers.A == 0, 0, 0, 0);
            break;
        } case 0xB2: {
            registers.A = registers.A | registers.D;
            set_flags(flag_register, registers.A == 0, 0, 0, 0);
            break;
        } case 0xB3: {
            registers.A = registers.A | registers.E;
            set_flags(flag_register, registers.A == 0, 0, 0, 0);
            break;
        } case 0xB4: {
            registers.A = registers.A | registers.H;
            set_flags(flag_register, registers.A == 0, 0, 0, 0);
            break;
        } case 0xB5: {
            registers.A = registers.A | registers.L;
            set_flags(flag_register, registers.A == 0, 0, 0, 0);
            break;
        } case 0xB6: {
            registers.A = registers.A | memory[HL.get()];
            set_flags(flag_register, registers.A == 0, 0, 0, 0);
            break;
        } case 0xB7: {
            registers.A = registers.A | registers.A;
            set_flags(flag_register, registers.A == 0, 0, 0, 0);
            break;
        } case 0xF6: {
            uint8_t nn = memory[registers.program_counter];
            registers.A = registers.A | nn;
            set_flags(flag_register, registers.A == 0, 0, 0, 0);
            registers.program_counter += 1;
            break;
        }
        default:
            std::cout << __FUNCTION__ << " Unknown opcode: " << (int)op_code << "\n";
            return;
    }
    return;
}

void ADC(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    switch (op_code) {
        case 0x88: {
            uint8_t c = flag_register.c;
            set_flags(flag_register, (uint8_t)(registers.A + registers.B + c) == 0, 0, is_half_carry_u8(registers.A, registers.B, c), is_carry_u8(registers.A, registers.B, c));
            registers.A = registers.A + registers.B + c;
            break;
        } case 0x89: {
            uint8_t c = flag_register.c;
            set_flags(flag_register, (uint8_t)(registers.A + registers.C + c) == 0, 0, is_half_carry_u8(registers.A, registers.C, c), is_carry_u8(registers.A, registers.C, c));
            registers.A = registers.A + registers.C + c;
            break;
        } case 0x8A: {
            uint8_t c = flag_register.c;
            set_flags(flag_register, (uint8_t)(registers.A + registers.D + c) == 0, 0, is_half_carry_u8(registers.A, registers.D, c), is_carry_u8(registers.A, registers.D, c));
            registers.A = registers.A + registers.D + c;
            break;
        } case 0x8B: {
            uint8_t c = flag_register.c;
            set_flags(flag_register, (uint8_t)(registers.A + registers.E + c) == 0, 0, is_half_carry_u8(registers.A, registers.E, c), is_carry_u8(registers.A, registers.E, c));
            registers.A = registers.A + registers.E + c;
            break;
        } case 0x8C: {
            uint8_t c = flag_register.c;
            set_flags(flag_register, (uint8_t)(registers.A + registers.H + c) == 0, 0, is_half_carry_u8(registers.A, registers.H, c), is_carry_u8(registers.A, registers.H, c));
            registers.A = registers.A + registers.H + c;
            break;
        } case 0x8D: {
            uint8_t c = flag_register.c;
            set_flags(flag_register, (uint8_t)(registers.A + registers.L + c) == 0, 0, is_half_carry_u8(registers.A, registers.L, c), is_carry_u8(registers.A, registers.L, c));
            registers.A = registers.A + registers.L + c;
            break;
        } case 0x8E: {
            uint8_t c = flag_register.c;
            set_flags(flag_register, (uint8_t)(registers.A + memory[HL.get()] + c) == 0, 0, is_half_carry_u8(registers.A, memory[HL.get()], c), is_carry_u8_u16(registers.A, memory[HL.get()], c));
            registers.A = registers.A + memory[HL.get()] + c;
            break;
        } case 0x8F: {
            uint8_t c = flag_register.c;
            set_flags(flag_register, (uint8_t)(registers.A + registers.A + c) == 0, 0, is_half_carry_u8(registers.A, registers.A, c), is_carry_u8(registers.A, registers.A, c));
            registers.A = registers.A + registers.A + c;
            break;
        } case 0xCE: {
            uint8_t c = flag_register.c;
            uint8_t nn = memory[registers.program_counter];
            set_flags(flag_register, (uint8_t)(registers.A + nn + c) == 0, 0, is_half_carry_u8(registers.A, nn, c), is_carry_u8(registers.A, nn, c));
            registers.A = registers.A + nn + c;
            registers.program_counter += 1;
            break;
        }
        default:
            std::cout << __FUNCTION__ << " Unknown opcode: " << (int)op_code << "\n";
            break;
    }
    return;
}

void LOAD(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    switch (op_code) {
        case 0x01: {
            BC.set(Virtual_Register(memory[registers.program_counter + 1], memory[registers.program_counter]).get());
            registers.program_counter += 2;
            break;
        } case 0x11: {
            DE.set(Virtual_Register(memory[registers.program_counter + 1], memory[registers.program_counter]).get());
            registers.program_counter += 2;
            break;
        } case 0x21: {
            HL.set(Virtual_Register(memory[registers.program_counter + 1], memory[registers.program_counter]).get());
            registers.program_counter += 2;
            break;
        } case 0x31: {
            registers.stack_pointer = Virtual_Register(memory[registers.program_counter + 1], memory[registers.program_counter]).get();
            registers.program_counter += 2;
            break;
        } case 0x02: {
            memory[BC.get()] = registers.A;
            break;
        } case 0x12: {
            memory[DE.get()] = registers.A;
            break;
        } case 0x22: {
            memory[HL.get()] = registers.A;
            HL.increment(1);
            break;
        } case 0x32: {
            memory[HL.get()] = registers.A;
            HL.increment(-1);
            break;
        } case 0x06: {
            uint8_t nn = memory[registers.program_counter];
            registers.B = nn;
            registers.program_counter += 1;
            break;
        } case 0x16: {
            uint8_t nn = memory[registers.program_counter];
            registers.D = nn;
            registers.program_counter += 1;
            break;
        } case 0x26: {
            uint8_t nn = memory[registers.program_counter];
            registers.H = nn;
            registers.program_counter += 1;
            break;
        } case 0x36: {
            uint8_t nn = memory[registers.program_counter];
            memory[HL.get()] = nn;
            registers.program_counter += 1;
            break;
        } case 0x0A: {
            registers.A = memory[BC.get()];
            break;
        } case 0x1A: {
            registers.A = memory[DE.get()];
            break;
        } case 0x2A: {
            registers.A = memory[HL.get()];
            HL.increment(1);
            break;
        } case 0x3A: {
            registers.A = memory[HL.get()];
            HL.increment(-1);
            break;
        } case 0x0E: {
            uint8_t nn = memory[registers.program_counter];
            registers.C = nn;
            registers.program_counter += 1;
            break;
        } case 0x1E: {
            uint8_t nn = memory[registers.program_counter];
            registers.E = nn;
            registers.program_counter += 1;
            break;
        } case 0x2E: {
            uint8_t nn = memory[registers.program_counter];
            registers.L = nn;
            registers.program_counter += 1;
            break;
        } case 0x3E: {
            uint8_t nn = memory[registers.program_counter];
            registers.A = nn;
            registers.program_counter += 1;
            break;
        } case 0x08: {
            uint16_t u16 = Virtual_Register(memory[registers.program_counter + 1], memory[registers.program_counter]).get();
            memory[u16] = (registers.stack_pointer & 0x00FF);
            memory[u16 + 1] = (registers.stack_pointer & 0xFF00) >> 8;
            registers.program_counter += 2;
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
            registers.B = memory[HL.get()];
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
            registers.C = memory[HL.get()];
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
            registers.D = memory[HL.get()];
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
            registers.E = memory[HL.get()];
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
            registers.H = memory[HL.get()];
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
            registers.L = memory[HL.get()];
            break;
        } case 0x6F: {
            registers.L = registers.A;
            break;
        } case 0x70: {
            // I am so confused how this is useful
            memory[HL.get()] = registers.B;
            break;
        } case 0x71: {
            memory[HL.get()] = registers.C;
            break;
        } case 0x72: {
            memory[HL.get()] = registers.D;
            break;
        } case 0x73: {
            memory[HL.get()] = registers.E;
            break;
        } case 0x74: {
            memory[HL.get()] = registers.H;
            break;
        } case 0x75: {
            memory[HL.get()] = registers.L;
            break;
        } case 0x77: {
            memory[HL.get()] = registers.A;
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
            registers.A = memory[HL.get()];
            break;
        } case 0x7F: {
            registers.A = registers.A;
            break;
        } case 0xE0: {
            uint8_t nn = memory[registers.program_counter];
            memory[0xFF00 + nn] = registers.A;
            registers.program_counter += 1;
            break;
        } case 0xE2: {
            memory[0xFF00 + registers.C] = registers.A;
            break;
        } case 0xEA: {
            // not sure if I implemented this right
            memory[Virtual_Register(memory[registers.program_counter + 1], memory[registers.program_counter]).get()]  = registers.A;
            registers.program_counter += 2;
            break;
        } case 0xF0: {
            uint8_t nn = memory[registers.program_counter];
            registers.A = memory[0xFF00 + nn];
            registers.program_counter += 1;
            break;
        } case 0xF2: {
            registers.A = memory[0xFF00 + registers.C];
            break;
        } case 0xF8: {
            std::int8_t nn = memory[registers.program_counter];
            set_flags(flag_register, 0, 0, (((registers.stack_pointer & 0xF) + (nn & 0xF)) > 0x0F), ((registers.stack_pointer & 0xFF) + (nn & 0xFF)) > 0xFF);
            HL.set(registers.stack_pointer + nn);
            registers.program_counter += 1;
            break;
        } case 0xF9: {
            registers.stack_pointer = HL.get();
            break;
        } case 0xFA: {
            registers.A = memory[Virtual_Register(memory[registers.program_counter + 1], memory[registers.program_counter]).get()];
            registers.program_counter += 2;
            break;
        }
        default:
            std::cout << __FUNCTION__ << " Unknown opcode: " << (int)op_code << "\n";
            break;
    }
    return;
}

void RLC(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    switch (memory[registers.program_counter]) {
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
            std::cout << __FUNCTION__ << " Unknown opcode: " << (int)op_code << "\n";
            break;
    }
    return;
}

void RRC(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    switch (memory[registers.program_counter]) {
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
            std::cout << __FUNCTION__ << " Unknown opcode: " << (int)op_code << "\n";
            break;
    }
    return;
}

void RL(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    switch (memory[registers.program_counter]) {
        case 0x10: {
            uint8_t carry = flag_register.c;
            uint8_t carry_bit = get_bit(registers.B, 7);
            rotate_left(registers.B);
            set_bit(registers.B, 0, carry);
            set_flags(flag_register, registers.B == 0, 0, 0, carry_bit);
            break;
        } case 0x11: {
            uint8_t carry = flag_register.c;
            uint8_t carry_bit = get_bit(registers.C, 7);
            rotate_left(registers.C);
            set_bit(registers.C, 0, carry);
            set_flags(flag_register, registers.C == 0, 0, 0, carry_bit);
            break;
        } case 0x12: {
            uint8_t carry = flag_register.c;
            uint8_t carry_bit = get_bit(registers.D, 7);
            rotate_left(registers.D);
            set_bit(registers.D, 0, carry);
            set_flags(flag_register, registers.D == 0, 0, 0, carry_bit);
            break;
        } case 0x13: {
            uint8_t carry = flag_register.c;
            uint8_t carry_bit = get_bit(registers.E, 7);
            rotate_left(registers.E);
            set_bit(registers.E, 0, carry);
            set_flags(flag_register, registers.E == 0, 0, 0, carry_bit);
            break;
        } case 0x14: {
            uint8_t carry = flag_register.c;
            uint8_t carry_bit = get_bit(registers.H, 7);
            rotate_left(registers.H);
            set_bit(registers.H, 0, carry);
            set_flags(flag_register, registers.H == 0, 0, 0, carry_bit);
            break;
        } case 0x15: {
            uint8_t carry = flag_register.c;
            uint8_t carry_bit = get_bit(registers.L, 7);
            rotate_left(registers.L);
            set_bit(registers.L, 0, carry);
            set_flags(flag_register, registers.L == 0, 0, 0, carry_bit);
            break;
        } case 0x16: {
            uint8_t carry = flag_register.c;
            uint8_t carry_bit = get_bit(memory[HL.get()], 7);
            rotate_left(memory[HL.get()]);
            set_bit(memory[HL.get()], 0, carry);
            set_flags(flag_register, memory[HL.get()] == 0, 0, 0, carry_bit);
            break;
        } case 0x17: {
            uint8_t carry = flag_register.c;
            uint8_t carry_bit = get_bit(registers.A, 7);
            rotate_left(registers.A);
            set_bit(registers.A, 0, carry);
            set_flags(flag_register, registers.A == 0, 0, 0, carry_bit);
            break;
        }
        default:
            std::cout << __FUNCTION__ << " Unknown opcode: " << (int)op_code << "\n";
            break;
    }
    return;
}

void RR(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    switch (memory[registers.program_counter]) {
        case 0x18: {
            uint8_t carry = flag_register.c;
            uint8_t carry_bit = get_bit(registers.B, 0);
            rotate_right(registers.B);
            set_bit(registers.B, 7, carry);
            set_flags(flag_register, registers.B == 0, 0, 0, carry_bit);
            break;
        } case 0x19: {
            uint8_t carry = flag_register.c;
            uint8_t carry_bit = get_bit(registers.C, 0);
            rotate_right(registers.C);
            set_bit(registers.C, 7, carry);
            set_flags(flag_register, registers.C == 0, 0, 0, carry_bit);
            break;
        } case 0x1A: {
            uint8_t carry = flag_register.c;
            uint8_t carry_bit = get_bit(registers.D, 0);
            rotate_right(registers.D);
            set_bit(registers.D, 7, carry);
            set_flags(flag_register, registers.D == 0, 0, 0, carry_bit);
            break;
        } case 0x1B: {
            uint8_t carry = flag_register.c;
            uint8_t carry_bit = get_bit(registers.E, 0);
            rotate_right(registers.E);
            set_bit(registers.E, 7, carry);
            set_flags(flag_register, registers.E == 0, 0, 0, carry_bit);
            break;
        } case 0x1C: {
            uint8_t carry = flag_register.c;
            uint8_t carry_bit = get_bit(registers.H, 0);
            rotate_right(registers.H);
            set_bit(registers.H, 7, carry);
            set_flags(flag_register, registers.H == 0, 0, 0, carry_bit);
            break;
        } case 0x1D: {
            uint8_t carry = flag_register.c;
            uint8_t carry_bit = get_bit(registers.L, 0);
            rotate_right(registers.L);
            set_bit(registers.L, 7, carry);
            set_flags(flag_register, registers.L == 0, 0, 0, carry_bit);
            break;
        } case 0x1E: {
            uint8_t carry = flag_register.c;
            uint8_t carry_bit = get_bit(memory[HL.get()], 0);
            rotate_right(memory[HL.get()]);
            set_bit(memory[HL.get()], 7, carry);
            set_flags(flag_register, memory[HL.get()] == 0, 0, 0, carry_bit);
            break;
        } case 0x1F: {
            uint8_t carry = flag_register.c;
            uint8_t carry_bit = get_bit(registers.A, 0);
            rotate_right(registers.A);
            set_bit(registers.A, 7, carry);
            set_flags(flag_register, registers.A == 0, 0, 0, carry_bit);
            break;
        }
        default:
            std::cout << __FUNCTION__ << " Unknown opcode: " << (int)op_code << "\n";
            break;
    }
    return;
}

void SLA(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    switch (memory[registers.program_counter]) {
        case 0x20: {
            uint8_t carry = get_bit(registers.B, 7);
            registers.B = (uint8_t)(registers.B << 1);
            set_flags(flag_register, registers.B == 0, 0, 0, carry);
            break;
        } case 0x21: {
            uint8_t carry = get_bit(registers.C, 7);
            registers.C = (uint8_t)(registers.C << 1);
            set_flags(flag_register, registers.C == 0, 0, 0, carry);
            break;
        } case 0x22: {
            uint8_t carry = get_bit(registers.D, 7);
            registers.D = (uint8_t)(registers.D << 1);
            set_flags(flag_register, registers.D == 0, 0, 0, carry);
            break;
        } case 0x23: {
            uint8_t carry = get_bit(registers.E, 7);
            registers.E = (uint8_t)(registers.E << 1);
            set_flags(flag_register, registers.E == 0, 0, 0, carry);
            break;
        } case 0x24: {
            uint8_t carry = get_bit(registers.H, 7);
            registers.H = (uint8_t)(registers.H << 1);
            set_flags(flag_register, registers.H == 0, 0, 0, carry);
            break;
        } case 0x25: {
            uint8_t carry = get_bit(registers.L, 7);
            registers.L = (uint8_t)(registers.L << 1);
            set_flags(flag_register, registers.L == 0, 0, 0, carry);
            break;
        } case 0x26: {
            uint8_t carry = get_bit(memory[HL.get()], 7);
            memory[HL.get()] = (uint8_t)(memory[HL.get()] << 1);
            set_flags(flag_register, memory[HL.get()] == 0, 0, 0, carry);
            break;
        } case 0x27: {
            uint8_t carry = get_bit(registers.A, 7);
            registers.A = (uint8_t)(registers.A << 1);
            set_flags(flag_register, registers.A == 0, 0, 0, carry);
            break;
        }
        default:
            std::cout << __FUNCTION__ << " Unknown opcode: " << (int)op_code << "\n";
            break;
    }
    return;
}

void SRA(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    switch (memory[registers.program_counter]) {
        case 0x28: {
            uint8_t carry = get_bit(registers.B, 0);
            registers.B = (uint8_t)((registers.B >> 1) | (registers.B & 0x80));
            set_flags(flag_register, registers.B == 0, 0, 0, carry);
            break;
        } case 0x29: {
            uint8_t carry = get_bit(registers.C, 0);
            registers.C = (uint8_t)((registers.C >> 1) | (registers.C & 0x80));
            set_flags(flag_register, registers.C == 0, 0, 0, carry);
            break;
        } case 0x2A: {
            uint8_t carry = get_bit(registers.D, 0);
            registers.D = (uint8_t)((registers.D >> 1) | (registers.D & 0x80));
            set_flags(flag_register, registers.D == 0, 0, 0, carry);
            break;
        } case 0x2B: {
            uint8_t carry = get_bit(registers.E, 0);
            registers.E = (uint8_t)((registers.E >> 1) | (registers.E & 0x80));
            set_flags(flag_register, registers.E == 0, 0, 0, carry);
            break;
        } case 0x2C: {
            uint8_t carry = get_bit(registers.H, 0);
            registers.H = (uint8_t)((registers.H >> 1) | (registers.H & 0x80));
            set_flags(flag_register, registers.H == 0, 0, 0, carry);
            break;
        } case 0x2D: {
            uint8_t carry = get_bit(registers.L, 0);
            registers.L = (uint8_t)((registers.L >> 1) | (registers.L & 0x80));
            set_flags(flag_register, registers.L == 0, 0, 0, carry);
            break;
        } case 0x2E: {
            uint8_t carry = get_bit(memory[HL.get()], 0);
            memory[HL.get()] = (uint8_t)((memory[HL.get()] >> 1) | (memory[HL.get()] & 0x80));
            set_flags(flag_register, memory[HL.get()] == 0, 0, 0, carry);
            break;
        } case 0x2F: {
            uint8_t carry = get_bit(registers.A, 0);
            registers.A = (uint8_t)((registers.A >> 1) | (registers.A & 0x80));
            set_flags(flag_register, registers.A == 0, 0, 0, carry);
            break;
        }
        default:
            std::cout << __FUNCTION__ << " Unknown opcode: " << (int)op_code << "\n";
            break;
    }
    return;
}

void SWAP(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    switch (memory[registers.program_counter]) {
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
            std::cout << __FUNCTION__ << " Unknown opcode: " << (int)op_code << "\n";
            break;
    }
    return;
}

void SRL(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    switch (memory[registers.program_counter]) {
        case 0x38: {
            uint8_t carry = get_bit(registers.B, 0);
            registers.B = (uint8_t)(registers.B >> 1);
            set_flags(flag_register, registers.B == 0, 0, 0, carry);
            break;
        } case 0x39: {
            uint8_t carry = get_bit(registers.C, 0);
            registers.C = (uint8_t)(registers.C >> 1);
            set_flags(flag_register, registers.C == 0, 0, 0, carry);
            break;
        } case 0x3A: {
            uint8_t carry = get_bit(registers.D, 0);
            registers.D = (uint8_t)(registers.D >> 1);
            set_flags(flag_register, registers.D == 0, 0, 0, carry);
            break;
        } case 0x3B: {
            uint8_t carry = get_bit(registers.E, 0);
            registers.E = (uint8_t)(registers.E >> 1);
            set_flags(flag_register, registers.E == 0, 0, 0, carry);
            break;
        } case 0x3C: {
            uint8_t carry = get_bit(registers.H, 0);
            registers.H = (uint8_t)(registers.H >> 1);
            set_flags(flag_register, registers.H == 0, 0, 0, carry);
            break;
        } case 0x3D: {
            uint8_t carry = get_bit(registers.L, 0);
            registers.L = (uint8_t)(registers.L >> 1);
            set_flags(flag_register, registers.L == 0, 0, 0, carry);
            break;
        } case 0x3E: {
            uint8_t carry = get_bit(memory[HL.get()], 0);
            memory[HL.get()] = (uint8_t)(memory[HL.get()] >> 1);
            set_flags(flag_register, memory[HL.get()] == 0, 0, 0, carry);
            break;
        } case 0x3F: {
            uint8_t carry = get_bit(registers.A, 0);
            registers.A = (uint8_t)(registers.A >> 1);
            set_flags(flag_register, registers.A == 0, 0, 0, carry);
            break;
        }
        default:
            std::cout << __FUNCTION__ << " Unknown opcode: " << (int)op_code << "\n";
            break;
    }
    return;
}

void BIT(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    switch (memory[registers.program_counter]) {
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
            std::cout << __FUNCTION__ << " Unknown opcode: " << (int)op_code << "\n";
            break;
    }
    return;
}

void RES(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    switch (memory[registers.program_counter]) {
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
            std::cout << __FUNCTION__ << " Unknown opcode: " << (int)op_code << "\n";
            break;
    }
    return;
}

void SET(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    switch (memory[registers.program_counter]) {
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
            std::cout << __FUNCTION__ << " Unknown opcode: " << (int)op_code << "\n";
            break;
    }
    return;
}

void cpu_cycle(std::vector<uint8_t> &memory, Flag_register &flag_register) {
    op_code = memory[registers.program_counter];
    std::cout << "A: " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (int)registers.A << " F: " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (int)(flag_register.z << 7 | flag_register.n << 6 | flag_register.h << 5 | flag_register.c << 4) << " B: " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (int)registers.B << " C: " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (int)registers.C << " D: " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (int)registers.D << " E: " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (int)registers.E << " H: " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (int)registers.H << " L: " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (int)registers.L << " SP: " << std::setw(4) << (int)registers.stack_pointer << " PC: 00:" << std::setw(4) << std::setfill('0') << std::uppercase << std::hex << registers.program_counter << " (" << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (int)op_code << " " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (int)memory[registers.program_counter + 1] << " " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (int)memory[registers.program_counter + 2] << " " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (int)memory[registers.program_counter + 3] << ")"<< std::endl;
    registers.program_counter++;
    switch (op_code & 0xF0) {
        case 0x00: {
            if (op_code == 0x00) {
                NOP(memory, flag_register);
            } else if (op_code == 0x01) {
                LOAD(memory, flag_register);
            } else if (op_code == 0x02) {
                LOAD(memory, flag_register);
            } else if (op_code == 0x03) {
                INC(memory, flag_register);
            } else if (op_code == 0x04) {
                INC(memory, flag_register);
            } else if (op_code == 0x05) {
                DEC(memory, flag_register);
            } else if (op_code == 0x06) {
                LOAD(memory, flag_register);
            } else if (op_code == 0x07) {
                RLCA(memory, flag_register);
            } else if (op_code == 0x08) {
                LOAD(memory, flag_register);
            } else if (op_code == 0x09) {
                ADD(memory, flag_register);
            } else if (op_code == 0x0A) {
                LOAD(memory, flag_register);
            } else if (op_code == 0x0B) {
                DEC(memory, flag_register);
            } else if (op_code == 0x0C) {
                INC(memory, flag_register);
            } else if (op_code == 0x0D) {
                DEC(memory, flag_register);
            } else if (op_code == 0x0E) {
                LOAD(memory, flag_register);
            } else if (op_code == 0x0F) {
                RRCA(memory, flag_register);
            }
            break;
        } case 0x10: {
            if (op_code == 0x10) {
                // stop doesn't have to be implemented
            } else if (op_code == 0x11) {
                LOAD(memory, flag_register);
            } else if (op_code == 0x12) {
                LOAD(memory, flag_register);
            } else if (op_code == 0x13) {
                INC(memory, flag_register);
            } else if (op_code == 0x14) {
                INC(memory, flag_register);
            } else if (op_code == 0x15) {
                DEC(memory, flag_register);
            } else if (op_code == 0x16) {
                LOAD(memory, flag_register);
            } else if (op_code == 0x17) {
                RLA(memory, flag_register);
            } else if (op_code == 0x18) {
                JR(memory, flag_register);
            } else if (op_code == 0x19) {
                ADD(memory, flag_register);
            } else if (op_code == 0x1A) {
                LOAD(memory, flag_register);
            } else if (op_code == 0x1B) {
                DEC(memory, flag_register);
            } else if (op_code == 0x1C) {
                INC(memory, flag_register);
            } else if (op_code == 0x1D) {
                DEC(memory, flag_register);
            } else if (op_code == 0x1E) {
                LOAD(memory, flag_register);
            } else if (op_code == 0x1F) {
                RRA(memory, flag_register);
            }
            break;
        } case 0x20: {
            if (op_code == 0x20) {
                JR(memory, flag_register);
            } else if (op_code == 0x21) {
                LOAD(memory, flag_register);
            } else if (op_code == 0x22) {
                LOAD(memory, flag_register);
            } else if (op_code == 0x23) {
                INC(memory, flag_register);
            } else if (op_code == 0x24) {
                INC(memory, flag_register);
            } else if (op_code == 0x25) {
                DEC(memory, flag_register);
            } else if (op_code == 0x26) {
                LOAD(memory, flag_register);
            } else if (op_code == 0x27) {
                DAA(memory, flag_register);
            } else if (op_code == 0x28) {
                JR(memory, flag_register);
            } else if (op_code == 0x29) {
                ADD(memory, flag_register);
            } else if (op_code == 0x2A) {
                LOAD(memory, flag_register);
            } else if (op_code == 0x2B) {
                DEC(memory, flag_register);
            } else if (op_code == 0x2C) {
                INC(memory, flag_register);
            } else if (op_code == 0x2D) {
                DEC(memory, flag_register);
            } else if (op_code == 0x2E) {
                LOAD(memory, flag_register);
            } else if (op_code == 0x2F) {
                CPL(memory, flag_register);
            }
            break;
        } case 0x30: {
            if (op_code == 0x30) {
                JR(memory, flag_register);
            } else if (op_code == 0x31) {
                LOAD(memory, flag_register);
            } else if (op_code == 0x32) {
                LOAD(memory, flag_register);
            } else if (op_code == 0x33) {
                INC(memory, flag_register);
            } else if (op_code == 0x34) {
                INC(memory, flag_register);
            } else if (op_code == 0x35) {
                DEC(memory, flag_register);
            } else if (op_code == 0x36) {
                LOAD(memory, flag_register);
            } else if (op_code == 0x37) {
                SCF(memory, flag_register);
            } else if (op_code == 0x38) {
                JR(memory, flag_register);
            } else if (op_code == 0x39) {
                ADD(memory, flag_register);
            } else if (op_code == 0x3A) {
                LOAD(memory, flag_register);
            } else if (op_code == 0x3B) {
                DEC(memory, flag_register);
            } else if (op_code == 0x3C) {
                INC(memory, flag_register);
            } else if (op_code == 0x3D) {
                DEC(memory, flag_register);
            } else if (op_code == 0x3E) {
                LOAD(memory, flag_register);
            } else if (op_code == 0x3F) {
                CCF(memory, flag_register);
            }
            break;
        } case 0x40: {
            LOAD(memory, flag_register);
            break;
        } case 0x50: {
            LOAD(memory, flag_register);
            break;
        } case 0x60: {
            LOAD(memory, flag_register);
            break;
        } case 0x70: {
            if (op_code == 0x76) {
                HALT(memory, flag_register);
            } else {
                LOAD(memory, flag_register);
            }
            break;
        } case 0x80: {
            if (op_code < 0x88) {
                ADD(memory, flag_register);
            } else {
                ADC(memory, flag_register);
            }
            break;
        } case 0x90: {
            if (op_code < 0x98) {
                SUB(memory, flag_register);
            } else {
                SBC(memory, flag_register);
            }
            break;
        } case 0xA0: {
            if (op_code < 0xA8) {
                AND(memory, flag_register);
            } else {
                XOR(memory, flag_register);
            }
            break;
        } case 0xB0: {
            if (0xB8 > op_code) {
                OR(memory, flag_register);
            } else {
                CP(memory, flag_register);
            }
            break;
        } case 0xC0: {
            if (op_code == 0xC0) {
                RET(memory, flag_register);
            } else if (op_code == 0xC1) {
                POP(memory, flag_register);
            } else if (op_code == 0xC2) {
                JP(memory, flag_register);
            } else if (op_code == 0xC3) {
                JP(memory, flag_register);
            } else if (op_code == 0xC4) {
                CALL(memory, flag_register);
            } else if (op_code == 0xC5) {
                PUSH(memory, flag_register);
            } else if (op_code == 0xC6) {
                ADD(memory, flag_register);
            } else if (op_code == 0xC7) {
                RST(memory, flag_register);
            } else if (op_code == 0xC8) {
                RET(memory, flag_register);
            } else if (op_code == 0xC9) {
                RET(memory, flag_register);
            } else if (op_code == 0xCA) {
                JP(memory, flag_register);
            } else if (op_code == 0xCB) {
                if (memory[registers.program_counter] < 0x08) {
                    RLC(memory, flag_register);
                } else if (memory[registers.program_counter] < 0x10) {
                    RRC(memory, flag_register);
                } else if (memory[registers.program_counter] < 0x18) {
                    RL(memory, flag_register);
                } else if (memory[registers.program_counter] < 0x20) {
                    RR(memory, flag_register);
                } else if (memory[registers.program_counter] < 0x28) {
                    SLA(memory, flag_register);
                } else if (memory[registers.program_counter] < 0x30) {
                    SRA(memory, flag_register);
                } else if (memory[registers.program_counter] < 0x38) {
                    SWAP(memory, flag_register);
                } else if (memory[registers.program_counter] < 0x40) {
                    SRL(memory, flag_register);
                } else if (memory[registers.program_counter] < 0x80) {
                    BIT(memory, flag_register);
                } else if (memory[registers.program_counter] < 0xC0) {
                    RES(memory, flag_register);
                } else {
                    SET(memory, flag_register);
                }
                registers.program_counter += 1;
            } else if (op_code == 0xCC) {
                CALL(memory, flag_register);
            } else if (op_code == 0xCD) {
                CALL(memory, flag_register);
            } else if (op_code == 0xCE) {
                ADC(memory, flag_register);
            } else if (op_code == 0xCF) {
                RST(memory, flag_register);
            }
            break;
        } case 0xD0: {
            if (op_code == 0xD0) {
                RET(memory, flag_register);
            } else if (op_code == 0xD1) {
                POP(memory, flag_register);
            } else if (op_code == 0xD2) {
                JP(memory, flag_register);
            } else if (op_code == 0xD4) {
                CALL(memory, flag_register);
            } else if (op_code == 0xD5) {
                PUSH(memory, flag_register);
            } else if (op_code == 0xD6) {
                SUB(memory, flag_register);
            } else if (op_code == 0xD7) {
                RST(memory, flag_register);
            } else if (op_code == 0xD8) {
                RET(memory, flag_register);
            } else if (op_code == 0xD9) {
                RETI(memory, flag_register);
            } else if (op_code == 0xDA) {
                JP(memory, flag_register);
            } else if (op_code == 0xDC) {
                CALL(memory, flag_register);
            } else if (op_code == 0xDE) {
                SBC(memory, flag_register);
            } else if (op_code == 0xDF) {
                RST(memory, flag_register);
            }
            break;
        } case 0xE0: {
            if (op_code == 0xE0) {
                LOAD(memory, flag_register);
            } else if (op_code == 0xE1) {
                POP(memory, flag_register);
            } else if (op_code == 0xE2) {
                LOAD(memory, flag_register);
            } else if (op_code == 0xE5) {
                PUSH(memory, flag_register);
            } else if (op_code == 0xE6) {
                AND(memory, flag_register);
            } else if (op_code == 0xE7) {
                RST(memory, flag_register);
            } else if (op_code == 0xE8) {
                ADD(memory, flag_register);
            } else if (op_code == 0xE9) {
                JP(memory, flag_register);
            } else if (op_code == 0xEA) {
                LOAD(memory, flag_register);
            } else if (op_code == 0xEE) {
                XOR(memory, flag_register);
            } else if (op_code == 0xEF) {
                RST(memory, flag_register);
            }
            break;
        } case 0xF0: {
            if (op_code == 0xF0) {
                LOAD(memory, flag_register);
            } else if (op_code == 0xF1) {
                POP(memory, flag_register);
            } else if (op_code == 0xF2) {
                LOAD(memory, flag_register);
            } else if (op_code == 0xF3) {
                DI(memory, flag_register);
            } else if (op_code == 0xF5) {
                PUSH(memory, flag_register);
            } else if (op_code == 0xF6) {
                OR(memory, flag_register);
            } else if (op_code == 0xF7) {
                RST(memory, flag_register);
            } else if (op_code == 0xF8) {
                LOAD(memory, flag_register);
            } else if (op_code == 0xF9) {
                LOAD(memory, flag_register);
            } else if (op_code == 0xFA) {
                LOAD(memory, flag_register);
            } else if (op_code == 0xFB) {
                EI(memory, flag_register);
            } else if (op_code == 0xFE) {
                CP(memory, flag_register);
            } else if (op_code == 0xFF) {
                RST(memory, flag_register);
            }
            break;
        }
        default:
            std::cout << __FUNCTION__ << " Unknown opcode: " << (int)op_code << "\n";
            break;
    }
    return;
}