pub fn get_bit(byte: u8, index: u8) -> bool {
    assert!((0 <= index) && (index <= 7));
    return ((byte >> index) & 1) == 1;
}

pub fn set_bit(byte: u8, index: u8, set: bool) -> u8 {
    assert!((0 <= index) && (index <= 7));
    if set {
        return byte | (1 << index);
    } else {
        return byte & !(1 << index);
    }
}

pub fn rotate_left(value: u8) -> u8 {
    (value << 1) | ((value >> 7) & 1)
}

pub fn rotate_right(value: u8) -> u8 {
    (value >> 1) | ((value & 1) << 7)
}

pub fn is_half_carry_u8(x: u8, y: u8, z: Option<u8>) -> bool {
    let z = match z {
        Some(z) => z,
        None => 0,
    };
    (((x & 0x0F) + (y & 0x0F) + z) & 0x10) == 0b10000
}

pub fn is_borrow_from_bit4(x: u8, y: u8, z: u8) -> bool {
    (x & 0xF) < ((y & 0xF) + z)
}
