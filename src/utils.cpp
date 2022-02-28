#include <cstdint>
#include <vector>
#include <assert.h>
#include <iostream>

std::uint8_t nibble_1(std::uint8_t byte) {
    return ((byte & 0xF0) >> 4);
}

std::uint8_t nibble_2(std::uint8_t byte) {
    return (byte & 0x0F);
}

bool is_half_carry_u8(std::uint8_t x, std::uint8_t y, std::uint8_t z = 0) {
    return (((x & 0x0F) + (y & 0x0F) + z) & 0x10) == 0b00010000;
}

bool is_carry_u8(std::uint8_t x, std::uint8_t y, std::uint8_t z = 0) {
    return (((std::uint16_t)x + (std::uint16_t)y + z) & 0x100) == 0b100000000;
}

bool is_half_carry_i8(std::uint8_t x, std::int8_t y) {
    return (((x & 0x0F) + (y & 0x0F)) & 0x10) == 0b00010000;
}

bool is_carry_i8(std::uint8_t x, std::int8_t y) {
    return (((std::uint16_t)x + (std::uint16_t)y) & 0x100) == 0b100000000;
}

bool is_carry_u8_u16(std::uint8_t x, std::uint16_t y, std::uint8_t z = 0) {
    return (((std::uint16_t)x + (y & 0xFF) + z) & 0x100) == 0b100000000;
}

bool is_half_carry_u16(std::uint16_t x, std::uint16_t y) {
    return (((x & 0x0FFF) + (y & 0x0FFF)) & 0x1000) == 0b1000000000000;
}

bool is_carry_u16(std::uint16_t x, std::uint16_t y) {
    return (((std::uint32_t)x + (std::uint32_t)y) & 0x10000) == 0b10000000000000000;
}

bool is_borrow_from_bit4(std::uint8_t x, std::uint8_t y) {
    return (x & 0x08) < (y & 0x08);
}

bool get_bit(std::uint8_t byte, std::uint8_t index) {
    assert(0 <= index <= 7);
    int i = 7 - index;
    return (byte & 1 << i) >> i;
}

void collect_digits(std::vector<std::uint8_t>& digits, std::uint8_t num) {
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