#include <cstdint>
#include <vector>
#include <assert.h>
#include <iostream>

uint8_t nibble_1(uint8_t byte) {
    return ((byte & 0xF0) >> 4);
}

uint8_t nibble_2(uint8_t byte) {
    return (byte & 0x0F);
}

bool is_half_carry_u8(uint8_t x, uint8_t y, uint8_t z = 0) {
    return (((x & 0x0F) + (y & 0x0F) + z) & 0x10) == 0b00010000;
}

bool is_carry_u8(uint8_t x, uint8_t y, uint8_t z = 0) {
    return (((std::uint16_t)x + (std::uint16_t)y + z) & 0x100) == 0b100000000;
}

bool is_half_carry_i8(uint8_t x, std::int8_t y) {
    return (((x & 0x0F) + (y & 0x0F)) & 0x10) == 0b00010000;
}

bool is_carry_i8(uint8_t x, std::int8_t y) {
    return (((std::uint16_t)x + (std::uint16_t)y) & 0x100) == 0b100000000;
}

bool is_carry_u8_u16(uint8_t x, std::uint16_t y, uint8_t z = 0) {
    return (((std::uint16_t)x + (y & 0xFF) + z) & 0x100) == 0b100000000;
}

bool is_half_carry_u16(std::uint16_t x, std::uint16_t y) {
    return (((x & 0x0FFF) + (y & 0x0FFF)) & 0x1000) == 0b1000000000000;
}

bool is_carry_u16(std::uint16_t x, std::uint16_t y) {
    return (((std::uint32_t)x + (std::uint32_t)y) & 0x10000) == 0b10000000000000000;
}

bool is_borrow_from_bit4(uint8_t x, uint8_t y, uint8_t z) {
    return ((x & 0xF) < ((y & 0xF) + z));
}

bool get_bit(uint8_t byte, uint8_t index) {
    assert(0 <= index <= 7);
    return (byte & 1 << index) >> index;
}

void set_bit(uint8_t& byte, uint8_t index, bool set) {
    assert(0 <= index <= 7);
    if (set) {
        byte = byte | (1 << index);
    } else {
        byte = byte & ~(1 << index);
    }
}

void rotate_left(uint8_t& byte) {
    byte = (byte << 1) | (bool)(byte >> 7);
}

void rotate_right(uint8_t& byte) {
    byte = (byte >> 1) | (uint8_t)(byte << 7);
}

void swap_nibbles(uint8_t& byte) {
    byte = ((byte & 0xF0) >> 4) | ((byte & 0x0F) << 4);
}

void collect_digits(std::vector<uint8_t>& digits, uint8_t num) {
    if (num > 9) {
        collect_digits(digits, num / 10);
    }
    digits.push_back(num % 10);
}

void show_usage(std::string name) {
    std::cerr << "Usage: " << name << " <option(s)>\n"
              << "Options:\n"
              << "\t-h,--help\t\tShow this help message\n"
              << "\t-d,--debug print debug messages into the console and go opcode by opcode on keyboard input"
              << std::endl;
}