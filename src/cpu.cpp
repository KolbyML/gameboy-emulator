//
// Created by kolby on 2/20/2022.
//

#include <cstdint>
#include <vector>
#include <iostream>
#include "cpu.h"

void add(std::vector<std::uint8_t> &memory, Registers &registers, Flag_register &flag_register) {
    switch (memory[registers.program_counter]) {
        case 0x80: {
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
            registers.A = registers.A + registers.B;
            break;
        } case 0x87: {
            registers.A = registers.A + registers.A;
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