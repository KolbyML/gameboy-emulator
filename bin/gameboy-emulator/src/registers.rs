use pixels::wgpu::core::registry;

#[derive(Debug, Clone, Copy, PartialEq)]
pub enum Register {
    A,
    F,
    B,
    C,
    D,
    E,
    H,
    L,
    AF,
    BC,
    DE,
    HL,
    SP,
}

impl Register {
    pub fn is_virtual(&self) -> bool {
        match self {
            Register::AF | Register::BC | Register::DE | Register::HL => true,
            _ => false,
        }
    }

    pub fn is_register(&self) -> bool {
        !self.is_virtual()
    }
}

pub struct Registers {
    pub a: u8,
    pub f: u8,
    pub b: u8,
    pub c: u8,
    pub d: u8,
    pub e: u8,
    pub h: u8,
    pub l: u8,
    pub stack_pointer: u16,
    pub program_counter: u16,
}

impl Registers {
    pub fn new() -> Self {
        Registers {
            a: 0x01,
            f: 0xb0,
            b: 0x00,
            c: 0x13,
            d: 0x00,
            e: 0xd8,
            h: 0x01,
            l: 0x4d,
            stack_pointer: 0xfffe,
            program_counter: 0x0100,
        }
    }

    pub fn get_virtual_register(&self, register: Register) -> u16 {
        match register {
            Register::AF => ((self.a as u16) << 8) | self.f as u16,
            Register::BC => ((self.b as u16) << 8) | self.c as u16,
            Register::DE => ((self.d as u16) << 8) | self.e as u16,
            Register::HL => ((self.h as u16) << 8) | self.l as u16,
            Register::SP => self.stack_pointer,
            _ => panic!("Invalid virtual register"),
        }
    }

    pub fn set_virtual_register(&mut self, register: Register, value: u16) {
        match register {
            Register::AF => {
                self.a = (value >> 8) as u8;
                self.f = value as u8;
            }
            Register::BC => {
                self.b = (value >> 8) as u8;
                self.c = value as u8;
            }
            Register::DE => {
                self.d = (value >> 8) as u8;
                self.e = value as u8;
            }
            Register::HL => {
                self.h = (value >> 8) as u8;
                self.l = value as u8;
            }
            Register::SP => self.stack_pointer = value,
            _ => panic!("Invalid virtual register"),
        }
    }

    /// Adds the values of two virtual registers and stores the result in the first register.
    pub fn add_virtual_registers(&mut self, register_x: Register, register_y: Register) {
        let x = self.get_virtual_register(register_x);
        let y = self.get_virtual_register(register_y);
        self.set_virtual_register(register_x, x.wrapping_add(y));
    }

    pub fn add_register(&mut self, register_x: Register, register_y: Register, z: Option<u8>) {
        let x = self.get_register(register_x);
        let y = self.get_register(register_y);
        let z = match z {
            Some(z) => z,
            None => 0,
        };
        self.set_register(register_x, x.wrapping_add(y).wrapping_add(z));
    }

    pub fn get_register(&self, register: Register) -> u8 {
        match register {
            Register::A => self.a,
            Register::F => self.f,
            Register::B => self.b,
            Register::C => self.c,
            Register::D => self.d,
            Register::E => self.e,
            Register::H => self.h,
            Register::L => self.l,
            _ => panic!("Invalid register {register:?}"),
        }
    }

    pub fn set_register(&mut self, register: Register, value: u8) {
        match register {
            Register::A => self.a = value,
            Register::F => self.f = value,
            Register::B => self.b = value,
            Register::C => self.c = value,
            Register::D => self.d = value,
            Register::E => self.e = value,
            Register::H => self.h = value,
            Register::L => self.l = value,
            _ => panic!("Invalid register"),
        }
    }

    pub fn increment_virtual_register(&mut self, register: Register) {
        let value = self.get_virtual_register(register);
        self.set_virtual_register(register, value.wrapping_add(1));
    }

    pub fn decrement_virtual_register(&mut self, register: Register) {
        let value = self.get_virtual_register(register);
        self.set_virtual_register(register, value.wrapping_sub(1));
    }

    pub fn increment_register(&mut self, register: Register) {
        let value = self.get_register(register);
        self.set_register(register, value.wrapping_add(1));
    }

    pub fn decrement_register(&mut self, register: Register) {
        let value = self.get_register(register);
        self.set_register(register, value.wrapping_sub(1));
    }

    pub fn get_bit(&self, register: Register, bit: u8) -> u8 {
        let value = match register {
            Register::A => self.a,
            Register::F => self.f,
            Register::B => self.b,
            Register::C => self.c,
            Register::D => self.d,
            Register::E => self.e,
            Register::H => self.h,
            Register::L => self.l,
            _ => panic!("Invalid register"),
        };
        (value >> bit) & 1
    }

    pub fn set_bit(&mut self, register: Register, bit: u8, value: bool) {
        let reg_value = match register {
            Register::A => &mut self.a,
            Register::F => &mut self.f,
            Register::B => &mut self.b,
            Register::C => &mut self.c,
            Register::D => &mut self.d,
            Register::E => &mut self.e,
            Register::H => &mut self.h,
            Register::L => &mut self.l,
            _ => panic!("Invalid register"),
        };
        if value {
            *reg_value |= 1 << bit;
        } else {
            *reg_value &= !(1 << bit);
        }
    }

    pub fn is_half_carry_u16(&self, register_x: Register, register_y: Register) -> bool {
        let x = self.get_virtual_register(register_x);
        let y = self.get_virtual_register(register_y);
        (((x & 0x0FFF) + (y & 0x0FFF)) & 0x1000) == 0b1000000000000
    }

    pub fn is_carry_u16(&self, register_x: Register, register_y: Register) -> bool {
        let x = self.get_virtual_register(register_x);
        let y = self.get_virtual_register(register_y);
        (((x as u32) + (y as u32)) & 0x10000) == 0b10000000000000000
    }

    pub fn is_carry_u8(&self, register_x: Register, y: u8, z: Option<u8>) -> bool {
        let x = self.get_register(register_x);
        let z = match z {
            Some(z) => z,
            None => 0,
        };
        (((x as u16) + (y as u16) + (z as u16)) & 0x100) == 0b100000000
    }

    pub fn is_added_zero(&self, register_x: Register, y: u8, z: Option<u8>) -> bool {
        let x = self.get_register(register_x);
        let z = match z {
            Some(z) => z as u16,
            None => 0,
        };
        ((x as u16) + (y as u16) + z) as u8 == 0
    }

    pub fn inc_pc(&mut self) {
        self.program_counter += 1;
    }

    pub fn dec_pc(&mut self) {
        self.program_counter -= 1;
    }

    pub fn inc_sp(&mut self) {
        self.stack_pointer += 1;
    }

    pub fn dec_sp(&mut self) {
        self.stack_pointer -= 1;
    }
}

#[derive(Debug, Clone, Copy)]
pub enum FlagRegister {
    Z,
    N,
    H,
    C,
    NZ,
    NC,
}

pub struct FlagRegisters {
    /// This bit is set if and only if the result of an operation is zero. Used by conditional jumps.
    pub z: u8,
    /// https://gbdev.io/pandocs/CPU_Registers_and_Flags.html#the-bcd-flags-n-h
    pub n: u8,
    pub h: u8,
    /// https://gbdev.io/pandocs/CPU_Registers_and_Flags.html#the-carry-flag-c-or-cy
    pub c: u8,
}

impl FlagRegisters {
    pub fn new() -> Self {
        FlagRegisters {
            z: 1,
            n: 0,
            h: 1,
            c: 1,
        }
    }

    pub fn set_flags(&mut self, z: i8, n: i8, h: i8, c: i8) {
        if z != -1 {
            self.z = z as u8;
        }
        if n != -1 {
            self.n = n as u8;
        }
        if h != -1 {
            self.h = h as u8;
        }
        if c != -1 {
            self.c = c as u8;
        }
    }
}
