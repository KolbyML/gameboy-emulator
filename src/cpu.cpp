//
// Created by kolby on 2/20/2022.
//

#include <cstdint>
#include <vector>
#include <iostream>
#include "cpu.h"
#include "utils.h"

void add(std::vector<std::uint8_t> &memory, Registers &registers, Flag_register &flag_register) {
    switch (memory[registers.program_counter]) {
        case 0x09: {
            set_u16_register(registers.H, registers.L, get_u16_register(registers.H, registers.L) + get_u16_register(registers.B, registers.C));
            break;
        } case 0x19: {
            set_u16_register(registers.H, registers.L, get_u16_register(registers.H, registers.L) + get_u16_register(registers.D, registers.E));
            break;
        } case 0x29: {
            set_u16_register(registers.H, registers.L, get_u16_register(registers.H, registers.L) + get_u16_register(registers.H, registers.L));
            break;
        } case 0x39: {
            set_u16_register(registers.H, registers.L, get_u16_register(registers.H, registers.L) + registers.stack_pointer);
            break;
        } case 0x80: {
            registers.A = registers.A + registers.B;
            break;
        } case 0x81: {
            registers.A = registers.A + registers.C;
            break;
        } case 0x82: {
            registers.A = registers.A + registers.D;
            break;
        } case 0x83: {
            registers.A = registers.A + registers.E;
            break;
        } case 0x84: {
            registers.A = registers.A + registers.H;
            break;
        } case 0x85: {
            registers.A = registers.A + registers.L;
            break;
        } case 0x86: {
            registers.A = registers.A + get_u16_register(registers.H, registers.L);
            break;
        } case 0x87: {
            registers.A = registers.A + registers.A;
            break;
        } case 0xC6: {
            // ?
            break;
        } case 0xE8: {
            // SP i8?
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
            registers.A = registers.A & registers.B;
            break;
        } case 0xA1: {
            registers.A = registers.A & registers.C;
            break;
        } case 0xA2: {
            registers.A = registers.A & registers.D;
            break;
        } case 0xA3: {
            registers.A = registers.A & registers.E;
            break;
        } case 0xA4: {
            registers.A = registers.A & registers.H;
            break;
        } case 0xA5: {
            registers.A = registers.A & registers.L;
            break;
        } case 0xA6: {
            registers.A = registers.A & get_u16_register(registers.H, registers.L);
            break;
        } case 0xA7: {
            registers.A = registers.A & registers.A;
            break;
        }
        default:
            std::cout << "Unknown opcode: " << memory[registers.program_counter] << "\n";
            return;
    }
    if (flag_register.z == 0) == flag_register.z = 1;
    flag_register.n = 0;
    flag_register.h = 0;
    flag_register.c = 0;
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