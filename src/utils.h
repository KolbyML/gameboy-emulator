//
// Created by kolby on 2/20/2022.
//

std::uint8_t nibble_1(std::uint8_t byte);
std::uint8_t nibble_2(std::uint8_t byte);
std::uint16_t get_u16_register(std::uint8_t x, std::uint8_t y);
void set_u16_register(std::uint8_t &x, std::uint8_t &y, std::uint16_t z);
bool get_bit(std::uint8_t byte, std::uint8_t index);
void collect_digits(std::vector<std::uint8_t>& digits, std::uint8_t num);
void show_usage(std::string name);
