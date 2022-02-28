#include <cstdint>

std::uint8_t nibble_1(std::uint8_t byte);
std::uint8_t nibble_2(std::uint8_t byte);
bool is_half_carry_u8(std::uint8_t x, std::uint8_t y, std::uint8_t z = 0);
bool is_carry_u8(std::uint8_t x, std::uint8_t y, std::uint8_t z = 0);
bool is_half_carry_i8(std::uint8_t x, std::int8_t y);
bool is_carry_i8(std::uint8_t x, std::int8_t y);
bool is_carry_u8_u16(std::uint8_t x, std::uint16_t y, std::uint8_t z = 0);
bool is_half_carry_u16(std::uint16_t x, std::uint16_t y);
bool is_carry_u16(std::uint16_t x, std::uint16_t y);
bool is_borrow_from_bit4(std::uint8_t x, std::uint8_t y);
bool get_bit(std::uint8_t byte, std::uint8_t index);
void collect_digits(std::vector<std::uint8_t>& digits, std::uint8_t num);
void show_usage(std::string name);
