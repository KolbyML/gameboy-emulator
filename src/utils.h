#include <cstdint>

uint8_t nibble_1(uint8_t byte);
uint8_t nibble_2(uint8_t byte);
bool is_half_carry_u8(uint8_t x, uint8_t y, uint8_t z = 0);
bool is_carry_u8(uint8_t x, uint8_t y, uint8_t z = 0);
bool is_half_carry_i8(uint8_t x, std::int8_t y);
bool is_carry_i8(uint8_t x, std::int8_t y);
bool is_carry_u8_u16(uint8_t x, std::uint16_t y, uint8_t z = 0);
bool is_half_carry_u16(std::uint16_t x, std::uint16_t y);
bool is_carry_u16(std::uint16_t x, std::uint16_t y);
bool is_borrow_from_bit4(uint8_t x, uint8_t y, uint8_t z = 0);
bool get_bit(uint8_t byte, uint8_t index);
void set_bit(uint8_t& byte, uint8_t index, bool set);
void rotate_left(uint8_t& byte);
void rotate_right(uint8_t& byte);
void swap_nibbles(uint8_t& byte);
void collect_digits(std::vector<uint8_t>& digits, uint8_t num);
void show_usage(std::string name);
