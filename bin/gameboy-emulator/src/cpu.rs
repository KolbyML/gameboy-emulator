use tracing::info;

use crate::{
    memory_bus::MemoryBus,
    registers::{FlagRegister, FlagRegisters, Register, Registers},
    utils::{get_bit, is_borrow_from_bit4, is_half_carry_u8, rotate_left, rotate_right, set_bit},
};

pub struct CPU {
    pub registers: Registers,
    pub flag_registers: FlagRegisters,
    pub memory_bus: MemoryBus,
}

impl CPU {
    const R_TABLE: [Register; 8] = [
        Register::B,
        Register::C,
        Register::D,
        Register::E,
        Register::H,
        Register::L,
        Register::HL,
        Register::A,
    ];
    const RP_TABLE: [Register; 4] = [Register::BC, Register::DE, Register::HL, Register::SP];
    const RP2_TABLE: [Register; 4] = [Register::BC, Register::DE, Register::HL, Register::AF];
    const CC_TABLE: [FlagRegister; 4] = [
        FlagRegister::NZ,
        FlagRegister::Z,
        FlagRegister::NC,
        FlagRegister::C,
    ];
    const ALU_TABLE: [fn(&mut CPU, u8); 8] = [
        CPU::add_a,
        CPU::adc_a,
        CPU::sub_a,
        CPU::sbc_a,
        CPU::and_a,
        CPU::xor_a,
        CPU::or_a,
        CPU::cp_a,
    ];

    const ROT_TABLE: [fn(&mut CPU, u8); 8] = [
        CPU::rotate_left_circular,
        CPU::rotate_right_circular,
        CPU::rotate_left,
        CPU::rotate_right,
        CPU::shift_left_arithmetic,
        CPU::shift_right_arithmetic,
        CPU::swap,
        CPU::shift_right_logically,
    ];

    pub fn new() -> Self {
        CPU {
            registers: Registers::new(),
            flag_registers: FlagRegisters::new(),
            memory_bus: MemoryBus::new(),
        }
    }

    fn fetch(&mut self) -> u8 {
        let opcode = self.memory_bus.read_byte(self.registers.program_counter);
        self.registers.program_counter = self.registers.program_counter.wrapping_add(1);
        opcode
    }

    fn fetch_word(&mut self) -> u16 {
        let lower_8bits = self.fetch() as u16;
        let upper_8bits = self.fetch() as u16;
        (upper_8bits << 8) | lower_8bits
    }

    fn fetch_stack_pointer(&mut self) -> u8 {
        let value = self.memory_bus.read_byte(self.registers.stack_pointer);
        self.registers.stack_pointer = self.registers.stack_pointer.wrapping_add(1);
        value
    }

    fn fetch_stack_pointer_word(&mut self) -> u16 {
        let value = self.memory_bus.read_word(self.registers.stack_pointer);
        self.registers.stack_pointer = self.registers.stack_pointer.wrapping_add(2);
        value
    }

    fn get_z_value(&self, z: u8) -> u8 {
        let z_register = CPU::R_TABLE[z as usize];
        if z_register.is_register() {
            self.registers.get_register(z_register)
        } else {
            self.memory_bus
                .read_byte(self.registers.get_virtual_register(z_register))
        }
    }

    fn set_z_value(&mut self, z: u8, value: u8) {
        let z_register = CPU::R_TABLE[z as usize];
        if z_register.is_register() {
            self.registers.set_register(z_register, value);
        } else {
            self.memory_bus
                .write_byte(self.registers.get_virtual_register(z_register), value);
        }
    }

    pub fn cpu_cycle(&mut self) {
        let opcode = self.fetch();
        self.registers.program_counter -= 1;
        // std::cout << "A: " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (int)registers.A << " F: " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (int)(flag_register.z << 7 | flag_register.n << 6 | flag_register.h << 5 | flag_register.c << 4) << " B: " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (int)registers.B << " C: " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (int)registers.C << " D: " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (int)registers.D << " E: " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (int)registers.E << " H: " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (int)registers.H << " L: " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (int)registers.L << " SP: " << std::setw(4) << (int)registers.stack_pointer << " PC: 00:" << std::setw(4) << std::setfill('0') << std::uppercase << std::hex << registers.program_counter << " (" << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (int)op_code << " " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (int)bus.get_memory(registers.program_counter + 1) << " " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (int)bus.get_memory(registers.program_counter + 2) << " " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (int)bus.get_memory(registers.program_counter + 3) << ")"<< std::endl;

        // println!(
        //     "A: {:02X} F: {:02X} B: {:02X} C: {:02X} D: {:02X} E: {:02X} H: {:02X} L: {:02X} SP: {:04X} PC: 00:{:04X} ({:02X} {:02X} {:02X} {:02X})",
        //     self.registers.a,
        //     self.flag_registers.z << 7 | self.flag_registers.n << 6 | self.flag_registers.h << 5 | self.flag_registers.c << 4,
        //     self.registers.b,
        //     self.registers.c,
        //     self.registers.d,
        //     self.registers.e,
        //     self.registers.h,
        //     self.registers.l,
        //     self.registers.stack_pointer,
        //     self.registers.program_counter,
        //     opcode,
        //     self.memory_bus.read_byte(self.registers.program_counter + 1),
        //     self.memory_bus.read_byte(self.registers.program_counter + 2),
        //     self.memory_bus.read_byte(self.registers.program_counter + 3)
        // );
        self.registers.program_counter += 1;

        // https://gb-archive.github.io/salvage/decoding_gbz80_opcodes/Decoding%20Gamboy%20Z80%20Opcodes.html
        let x: u8 = (opcode & 0b1100_0000) >> 6;
        let y: u8 = (opcode & 0b0011_1000) >> 3;
        let z: u8 = opcode & 0b0000_0111;
        let p: u8 = y >> 1;
        let q = y % 2;

        match (x, y, z, p, q) {
            (0, 0, 0, _, _) => self.no_operation(),
            (0, 1, 0, _, _) => self.load_nn_sp(),
            (0, 2, 0, _, _) => self.stop(),
            (0, 3, 0, _, _) => self.relative_jump_d(),
            (0, y, 0, _, _) => self.relative_jump_cc_d(y),
            (0, _, 1, p, 0) => self.load_immediate_16bit(p),
            (0, _, 1, p, 1) => self.add_hl_16bit(p),
            (0, _, 2, 0, 0) => self.indirect_loading_bc_a(),
            (0, _, 2, 1, 0) => self.indirect_loading_de_a(),
            (0, _, 2, 2, 0) => self.indirect_loading_hl_plus_a(),
            (0, _, 2, 3, 0) => self.indirect_loading_hl_minus_a(),
            (0, _, 2, 0, 1) => self.indirect_loading_a_bc(),
            (0, _, 2, 1, 1) => self.indirect_loading_a_de(),
            (0, _, 2, 2, 1) => self.indirect_loading_a_hl_plus(),
            (0, _, 2, 3, 1) => self.indirect_loading_a_hl_minus(),
            (0, _, 3, p, 0) => self.increment_16bit(p),
            (0, y, 4, _, _) => self.increment_8bit(y),
            (0, _, 3, p, 1) => self.decrement_16bit(p),
            (0, y, 5, _, _) => self.decrement_8bit(y),
            (0, y, 6, _, _) => self.load_immediate_8bit(y),
            (0, 0, 7, _, _) => self.rotate_left_circular_accumulator(),
            (0, 1, 7, _, _) => self.rotate_right_circular_accumulator(),
            (0, 2, 7, _, _) => self.rotate_left_accumulator(),
            (0, 3, 7, _, _) => self.rotate_right_accumulator(),
            (0, 4, 7, _, _) => self.decimal_adjust_accumulator(),
            (0, 5, 7, _, _) => self.complement_accumulator(),
            (0, 6, 7, _, _) => self.set_carry_flag(),
            (0, 7, 7, _, _) => self.complement_carry_flag(),
            (1, 6, 6, _, _) => (),
            (1, y, z, _, _) => self.load_y_z(y, z),
            (2, y, z, _, _) => {
                let value_z = self.get_z_value(z);
                Self::ALU_TABLE[y as usize](self, value_z)
            }
            (3, 4, 0, _, _) => self.load_ff00_plus_n_a(),
            (3, 5, 0, _, _) => self.add_sp_d(),
            (3, 6, 0, _, _) => self.load_a_ff00_plus_n(),
            (3, 7, 0, _, _) => self.load_hl_sp_plus_d(),
            (3, y, 0, _, _) => self.ret_cc(y),
            (3, _, 1, p, 0) => self.pop(p),
            (3, _, 1, 0, 1) => self.ret(),
            (3, _, 1, 1, 1) => self.reti(),
            (3, _, 1, 2, 1) => self.jp_hl(),
            (3, _, 1, 3, 1) => self.ld_sp_hl(),
            (3, 4, 2, _, _) => self.ld_ff00_plus_c_a(),
            (3, 5, 2, _, _) => self.ld_nn_a(),
            (3, 6, 2, _, _) => self.ld_a_ff00_plus_c(),
            (3, 7, 2, _, _) => self.ld_a_nn(),
            (3, y, 2, _, _) => self.jp_cc_nn(y),
            (3, 0, 3, _, _) => self.jp_nn(),
            (3, 1, 3, _, _) => {
                let opcode = self.fetch();
                let x: u8 = (opcode & 0b1100_0000) >> 6;
                let y: u8 = (opcode & 0b0011_1000) >> 3;
                let z: u8 = opcode & 0b0000_0111;

                match (x, y, z) {
                    (0, y, z) => Self::ROT_TABLE[y as usize](self, z),
                    (1, y, z) => self.bit(y, z),
                    (2, y, z) => self.res(y, z),
                    (3, y, z) => self.set(y, z),
                    _ => (),
                }
            }
            (3, 6, 3, _, _) => self.di(),
            (3, 7, 3, _, _) => self.ei(),
            (3, 0..=3, 4, _, _) => self.call_cc_nn(y),
            (3, _, 5, p, 0) => self.push_rp2(p),
            (3, _, 5, 0, 1) => self.call_nn(),
            (3, y, 6, _, _) => {
                let n = self.fetch();
                Self::ALU_TABLE[y as usize](self, n);
            }
            (3, y, 7, _, _) => self.rst(y),
            _ => (),
        }
    }

    fn no_operation(&mut self) {}

    fn load_nn_sp(&mut self) {
        let u16 = self.fetch_word();
        self.memory_bus
            .write_word(u16, self.registers.stack_pointer);
    }

    /// stop doesn't have to be implemented
    fn stop(&mut self) {}

    fn relative_jump_d(&mut self) {
        let d = self.memory_bus.read_byte(self.registers.program_counter) as i8;
        self.registers.program_counter = (self.registers.program_counter as i32 + d as i32) as u16;
        self.registers.program_counter = self.registers.program_counter.wrapping_add(1);
    }

    fn relative_jump_cc_d(&mut self, y: u8) {
        let d = self.memory_bus.read_byte(self.registers.program_counter) as i8;
        let cc = CPU::CC_TABLE[(y - 4) as usize];
        let condition = match cc {
            FlagRegister::NZ => self.flag_registers.z == 0,
            FlagRegister::Z => self.flag_registers.z == 1,
            FlagRegister::NC => self.flag_registers.c == 0,
            FlagRegister::C => self.flag_registers.c == 1,
            _ => unreachable!("CC_TABLE is not exhaustive"),
        };
        if condition {
            self.registers.program_counter =
                (self.registers.program_counter as i32 + d as i32) as u16;
        }
        self.registers.program_counter = self.registers.program_counter.wrapping_add(1);
    }

    fn load_immediate_16bit(&mut self, p: u8) {
        let register = CPU::RP_TABLE[p as usize];
        let nn = self.fetch_word();
        self.registers.set_virtual_register(register, nn);
    }

    fn add_hl_16bit(&mut self, p: u8) {
        let register = CPU::RP_TABLE[p as usize];
        self.flag_registers.set_flags(
            -1,
            0,
            self.registers.is_half_carry_u16(Register::HL, register) as i8,
            self.registers.is_carry_u16(Register::HL, register) as i8,
        );
        self.registers.add_virtual_registers(Register::HL, register);
    }

    fn indirect_loading_bc_a(&mut self) {
        let address = self.registers.get_virtual_register(Register::BC);
        self.memory_bus.write_byte(address, self.registers.a);
    }

    fn indirect_loading_de_a(&mut self) {
        let address = self.registers.get_virtual_register(Register::DE);
        self.memory_bus.write_byte(address, self.registers.a);
    }

    fn indirect_loading_hl_plus_a(&mut self) {
        let address = self.registers.get_virtual_register(Register::HL);
        self.memory_bus.write_byte(address, self.registers.a);
        self.registers.increment_virtual_register(Register::HL);
    }

    fn indirect_loading_hl_minus_a(&mut self) {
        let address = self.registers.get_virtual_register(Register::HL);
        self.memory_bus.write_byte(address, self.registers.a);
        self.registers.decrement_virtual_register(Register::HL);
    }

    fn indirect_loading_a_bc(&mut self) {
        let address = self.registers.get_virtual_register(Register::BC);
        self.registers.a = self.memory_bus.read_byte(address);
    }

    fn indirect_loading_a_de(&mut self) {
        let address = self.registers.get_virtual_register(Register::DE);
        self.registers.a = self.memory_bus.read_byte(address);
    }

    fn indirect_loading_a_hl_plus(&mut self) {
        let address = self.registers.get_virtual_register(Register::HL);
        self.registers.a = self.memory_bus.read_byte(address);
        self.registers.increment_virtual_register(Register::HL);
    }

    fn indirect_loading_a_hl_minus(&mut self) {
        let address = self.registers.get_virtual_register(Register::HL);
        self.registers.a = self.memory_bus.read_byte(address);
        self.registers.decrement_virtual_register(Register::HL);
    }

    fn increment_16bit(&mut self, p: u8) {
        let register = CPU::RP_TABLE[p as usize];
        self.registers.increment_virtual_register(register);
    }

    fn increment_8bit(&mut self, y: u8) {
        let value = self.get_z_value(y);
        self.flag_registers.set_flags(
            (value.wrapping_add(1) == 0) as i8,
            0,
            (is_half_carry_u8(value, 1, None)) as i8,
            -1,
        );
        self.set_z_value(y, value.wrapping_add(1));
    }

    fn decrement_16bit(&mut self, p: u8) {
        let register = CPU::RP_TABLE[p as usize];
        self.registers.decrement_virtual_register(register);
    }

    fn decrement_8bit(&mut self, y: u8) {
        let value = self.get_z_value(y);
        self.flag_registers.set_flags(
            (value.wrapping_sub(1) == 0) as i8,
            1,
            (is_borrow_from_bit4(value, 1, 0)) as i8,
            -1,
        );
        self.set_z_value(y, value.wrapping_sub(1));
    }

    fn load_immediate_8bit(&mut self, y: u8) {
        let value = self.fetch();
        self.set_z_value(y, value);
    }

    fn rotate_left_circular_accumulator(&mut self) {
        self.flag_registers
            .set_flags(0, 0, 0, self.registers.get_bit(Register::A, 7) as i8);
        self.registers
            .set_register(Register::A, rotate_left(self.registers.a));
    }

    fn rotate_right_circular_accumulator(&mut self) {
        self.flag_registers
            .set_flags(0, 0, 0, self.registers.get_bit(Register::A, 0) as i8);
        self.registers
            .set_register(Register::A, rotate_right(self.registers.a));
    }

    fn rotate_left_accumulator(&mut self) {
        let carry = self.flag_registers.c == 1;
        self.flag_registers
            .set_flags(0, 0, 0, self.registers.get_bit(Register::A, 7) as i8);
        let mut value_a = self.registers.a;
        value_a = rotate_left(value_a);
        value_a = set_bit(value_a, 0, carry);
        self.registers.a = value_a;
    }

    fn rotate_right_accumulator(&mut self) {
        let carry = self.flag_registers.c == 1;
        self.flag_registers
            .set_flags(0, 0, 0, self.registers.get_bit(Register::A, 0) as i8);
        let mut value_a = self.registers.a;
        value_a = rotate_right(value_a);
        value_a = set_bit(value_a, 7, carry);
        self.registers.a = value_a;
    }

    fn decimal_adjust_accumulator(&mut self) {
        if self.flag_registers.n == 0 {
            if self.flag_registers.c != 0 || self.registers.a > 0x99 {
                self.registers.a = self.registers.a.wrapping_add(0x60);
                self.flag_registers.c = 1;
            }
            if self.flag_registers.h != 0 || (self.registers.a & 0x0f) > 0x09 {
                self.registers.a = self.registers.a.wrapping_add(0x60);
            }
        } else {
            if self.flag_registers.c != 0 {
                self.registers.a = self.registers.a.wrapping_sub(0x60);
            }
            if self.flag_registers.h != 0 {
                self.registers.a = self.registers.a.wrapping_sub(0x60);
            }
        }

        self.flag_registers.z = (self.registers.a == 0) as u8;
        self.flag_registers.h = 0;
    }

    fn complement_accumulator(&mut self) {
        self.flag_registers.set_flags(-1, 1, 1, -1);
        self.registers.a = !self.registers.a;
    }

    fn set_carry_flag(&mut self) {
        self.flag_registers.set_flags(-1, 0, 0, 1);
    }

    fn complement_carry_flag(&mut self) {
        self.flag_registers
            .set_flags(-1, 0, 0, (self.flag_registers.c ^ 1) as i8);
    }

    fn load_y_z(&mut self, y: u8, z: u8) {
        let value = self.get_z_value(z);
        self.set_z_value(y, value);
    }

    fn add_a(&mut self, value: u8) {
        let value_a = self.registers.get_register(Register::A);
        self.flag_registers.set_flags(
            self.registers.is_added_zero(Register::A, value, None) as i8,
            0,
            is_half_carry_u8(value_a, value, None) as i8,
            self.registers.is_carry_u8(Register::A, value, None) as i8,
        );
        self.registers
            .set_register(Register::A, value_a.wrapping_add(value));
    }

    fn adc_a(&mut self, value: u8) {
        let c = self.flag_registers.c;
        let value_a = self.registers.get_register(Register::A);
        self.flag_registers.set_flags(
            self.registers.is_added_zero(Register::A, value, Some(c)) as i8,
            0,
            is_half_carry_u8(value_a, value, Some(c)) as i8,
            self.registers.is_carry_u8(Register::A, value, Some(c)) as i8,
        );
        self.registers
            .set_register(Register::A, value_a.wrapping_add(value).wrapping_add(c));
    }

    fn sub_a(&mut self, value: u8) {
        let value_a = self.registers.get_register(Register::A);
        self.flag_registers.set_flags(
            (value_a.wrapping_sub(value) == 0) as i8,
            1,
            (is_borrow_from_bit4(value_a, value, 0)) as i8,
            (value > value_a) as i8,
        );
        self.registers
            .set_register(Register::A, value_a.wrapping_sub(value));
    }

    fn sbc_a(&mut self, value: u8) {
        let value_a = self.registers.get_register(Register::A);
        let c = self.flag_registers.c;
        self.flag_registers.set_flags(
            (value_a.wrapping_sub(value).wrapping_sub(c) == 0) as i8,
            1,
            (is_borrow_from_bit4(value_a, value, c)) as i8,
            (value.wrapping_add(c) > value_a) as i8,
        );
        self.registers
            .set_register(Register::A, value_a.wrapping_sub(value).wrapping_sub(c));
    }

    fn and_a(&mut self, value: u8) {
        let mut value_a = self.registers.get_register(Register::A);
        value_a &= value;
        self.flag_registers.set_flags((value_a == 0) as i8, 0, 1, 0);
        self.registers.set_register(Register::A, value_a);
    }

    fn xor_a(&mut self, value: u8) {
        let mut value_a = self.registers.get_register(Register::A);
        value_a ^= value;
        self.flag_registers.set_flags((value_a == 0) as i8, 0, 0, 0);
        self.registers.set_register(Register::A, value_a);
    }

    fn or_a(&mut self, value: u8) {
        let mut value_a = self.registers.get_register(Register::A);
        value_a |= value;
        self.flag_registers.set_flags((value_a == 0) as i8, 0, 0, 0);
        self.registers.set_register(Register::A, value_a);
    }

    fn cp_a(&mut self, value: u8) {
        let value_a = self.registers.get_register(Register::A);
        self.flag_registers.set_flags(
            (value_a.wrapping_sub(value) == 0) as i8,
            1,
            (is_borrow_from_bit4(value_a, value, 0)) as i8,
            (value > value_a) as i8,
        );
    }

    fn ret_cc(&mut self, y: u8) {
        let cc = CPU::CC_TABLE[y as usize];
        let condition = match cc {
            FlagRegister::NZ => self.flag_registers.z == 0,
            FlagRegister::Z => self.flag_registers.z == 1,
            FlagRegister::NC => self.flag_registers.c == 0,
            FlagRegister::C => self.flag_registers.c == 1,
            _ => unreachable!("CC_TABLE is not exhaustive"),
        };
        if condition {
            self.registers.program_counter = self.fetch_stack_pointer_word();
        }
    }

    fn load_ff00_plus_n_a(&mut self) {
        let n = self.fetch();
        let address = 0xff00 + n as u16;
        self.memory_bus.write_byte(address, self.registers.a);
    }

    fn add_sp_d(&mut self) {
        let d = self.fetch() as i8 as i32;
        let sp = self.registers.stack_pointer as i32;
        self.flag_registers.set_flags(
            0,
            0,
            (((sp & 0xf) + (d & 0x0f)) > 0x0f) as i8,
            (((sp & 0xff) + (d & 0xff)) > 0xff) as i8,
        );
        self.registers.stack_pointer = (sp + d) as u16;
    }

    fn load_a_ff00_plus_n(&mut self) {
        let n = self.fetch();
        let address = 0xff00 + n as u16;
        self.registers.a = self.memory_bus.read_byte(address);
    }

    fn load_hl_sp_plus_d(&mut self) {
        let d = self.fetch() as i8 as i32;
        let sp = self.registers.stack_pointer as i32;
        self.flag_registers.set_flags(
            0,
            0,
            (((sp & 0xf) + (d & 0x0f)) > 0x0f) as i8,
            (((sp & 0xff) + (d & 0xff)) > 0xff) as i8,
        );
        self.registers
            .set_virtual_register(Register::HL, (sp + d) as u16);
    }

    fn pop(&mut self, p: u8) {
        let register = CPU::RP2_TABLE[p as usize];
        if Register::AF == register {
            let reg_f = self.fetch_stack_pointer();
            self.flag_registers.set_flags(
                get_bit(reg_f, 7) as i8,
                get_bit(reg_f, 6) as i8,
                get_bit(reg_f, 5) as i8,
                get_bit(reg_f, 4) as i8,
            );
            self.registers.a = self.fetch_stack_pointer();
        } else {
            let word = self.fetch_stack_pointer_word();
            self.registers.set_virtual_register(register, word);
        }
    }

    fn ret(&mut self) {
        self.registers.program_counter = self.fetch_stack_pointer_word();
    }

    fn reti(&mut self) {
        self.registers.program_counter = self.fetch_stack_pointer_word();
        self.memory_bus.ime = true;
    }

    fn jp_hl(&mut self) {
        self.registers.program_counter = self.registers.get_virtual_register(Register::HL);
    }

    fn ld_sp_hl(&mut self) {
        self.registers.stack_pointer = self.registers.get_virtual_register(Register::HL);
    }

    fn ld_ff00_plus_c_a(&mut self) {
        let address = 0xff00 + self.registers.c as u16;
        self.memory_bus.write_byte(address, self.registers.a);
    }

    fn ld_nn_a(&mut self) {
        let nn = self.fetch_word();
        self.memory_bus.write_byte(nn, self.registers.a);
    }

    fn ld_a_ff00_plus_c(&mut self) {
        let address = 0xff00 + self.registers.c as u16;
        self.registers.a = self.memory_bus.read_byte(address);
    }

    fn ld_a_nn(&mut self) {
        let nn = self.fetch_word();
        self.registers.a = self.memory_bus.read_byte(nn);
    }

    fn jp_cc_nn(&mut self, y: u8) {
        let nn = self.fetch_word();
        let cc = CPU::CC_TABLE[y as usize];
        let condition = match cc {
            FlagRegister::NZ => self.flag_registers.z == 0,
            FlagRegister::Z => self.flag_registers.z == 1,
            FlagRegister::NC => self.flag_registers.c == 0,
            FlagRegister::C => self.flag_registers.c == 1,
            _ => unreachable!("CC_TABLE is not exhaustive"),
        };
        if condition {
            self.registers.program_counter = nn;
        } else {
            self.registers.program_counter = self.registers.program_counter.wrapping_add(2);
        }
    }

    fn jp_nn(&mut self) {
        let nn = self.fetch_word();
        self.registers.program_counter = nn;
    }

    fn di(&mut self) {
        self.memory_bus.ime = false;
    }

    fn ei(&mut self) {
        self.memory_bus.ime = true;
        self.memory_bus.count = 0;
    }

    fn call_cc_nn(&mut self, y: u8) {
        let cc = CPU::CC_TABLE[y as usize];
        let condition = match cc {
            FlagRegister::NZ => self.flag_registers.z == 0,
            FlagRegister::Z => self.flag_registers.z == 1,
            FlagRegister::NC => self.flag_registers.c == 0,
            FlagRegister::C => self.flag_registers.c == 1,
            _ => unreachable!("CC_TABLE is not exhaustive"),
        };
        if condition {
            self.registers.stack_pointer = self.registers.stack_pointer.wrapping_sub(1);
            self.memory_bus.write_byte(
                self.registers.stack_pointer,
                (((self.registers.program_counter + 2) & 0xFF00) >> 8) as u8,
            );
            self.registers.stack_pointer = self.registers.stack_pointer.wrapping_sub(1);
            self.memory_bus.write_byte(
                self.registers.stack_pointer,
                ((self.registers.program_counter + 2) & 0x00FF) as u8,
            );
            let nnnn = self.fetch_word();
            self.registers.program_counter = nnnn;
        } else {
            self.registers.program_counter += 2;
        }
    }

    fn push_rp2(&mut self, p: u8) {
        let register = CPU::RP2_TABLE[p as usize];
        if register == Register::AF {
            self.registers.stack_pointer = self.registers.stack_pointer.wrapping_sub(1);
            self.memory_bus
                .write_byte(self.registers.stack_pointer, self.registers.a);
            self.registers.stack_pointer = self.registers.stack_pointer.wrapping_sub(1);
            self.memory_bus.write_byte(
                self.registers.stack_pointer,
                self.flag_registers.z << 7
                    | self.flag_registers.n << 6
                    | self.flag_registers.h << 5
                    | self.flag_registers.c << 4,
            );
        } else {
            self.registers.stack_pointer = self.registers.stack_pointer.wrapping_sub(2);
            self.memory_bus.write_word(
                self.registers.stack_pointer,
                self.registers.get_virtual_register(register),
            );
        }
    }

    fn call_nn(&mut self) {
        self.registers.stack_pointer = self.registers.stack_pointer.wrapping_sub(1);
        self.memory_bus.write_byte(
            self.registers.stack_pointer,
            (((self.registers.program_counter + 2) & 0xFF00) >> 8) as u8,
        );
        self.registers.stack_pointer = self.registers.stack_pointer.wrapping_sub(1);
        self.memory_bus.write_byte(
            self.registers.stack_pointer,
            ((self.registers.program_counter + 2) & 0x00FF) as u8,
        );
        let nnnn = self.fetch_word();
        self.registers.program_counter = nnnn;
    }

    fn rst(&mut self, y: u8) {
        self.registers.stack_pointer = self.registers.stack_pointer.wrapping_sub(2);
        self.memory_bus.write_word(
            self.registers.stack_pointer,
            self.registers.program_counter + 1,
        );
        self.registers.program_counter = y as u16 * 8;
    }

    fn rotate_left_circular(&mut self, z: u8) {
        let value_z = self.get_z_value(z);
        let value_z = rotate_left(value_z);
        self.set_z_value(z, value_z);
        self.flag_registers
            .set_flags((value_z == 0) as i8, 0, 0, get_bit(value_z, 7) as i8);
    }

    fn rotate_right_circular(&mut self, z: u8) {
        let value_z = self.get_z_value(z);
        let value_z = rotate_right(value_z);
        self.set_z_value(z, value_z);
        self.flag_registers
            .set_flags((value_z == 0) as i8, 0, 0, get_bit(value_z, 0) as i8);
    }

    fn rotate_left(&mut self, z: u8) {
        let carry = self.flag_registers.c == 1;
        let value_z = self.get_z_value(z);
        let carry_bit = get_bit(value_z, 7);
        let value_z = rotate_left(value_z);
        let value_z = set_bit(value_z, 0, carry);
        self.flag_registers
            .set_flags((value_z == 0) as i8, 0, 0, carry_bit as i8);
        self.set_z_value(z, value_z);
    }

    fn rotate_right(&mut self, z: u8) {
        let carry = self.flag_registers.c == 1;
        let value_z = self.get_z_value(z);
        let carry_bit = get_bit(value_z, 0);
        let value_z = rotate_right(value_z);
        let value_z = set_bit(value_z, 7, carry);
        self.flag_registers
            .set_flags((value_z == 0) as i8, 0, 0, carry_bit as i8);
        self.set_z_value(z, value_z);
    }

    fn shift_left_arithmetic(&mut self, z: u8) {
        let value_z = self.get_z_value(z);
        let carry = get_bit(value_z, 7);
        let result = value_z << 1;
        self.flag_registers
            .set_flags((result == 0) as i8, 0, 0, carry as i8);
        self.set_z_value(z, result);
    }

    fn shift_right_arithmetic(&mut self, z: u8) {
        let value_z = self.get_z_value(z);
        let carry = get_bit(value_z, 0);
        let result = value_z >> 1;
        self.flag_registers
            .set_flags((result == 0) as i8, 0, 0, carry as i8);
        self.set_z_value(z, result);
    }

    fn swap(&mut self, z: u8) {
        let value_z = self.get_z_value(z);
        let result = (value_z << 4) | (value_z >> 4);
        self.flag_registers.set_flags((result == 0) as i8, 0, 0, 0);
        self.set_z_value(z, result);
    }

    fn shift_right_logically(&mut self, z: u8) {
        let value_z = self.get_z_value(z);
        let carry = get_bit(value_z, 0);
        let result = value_z >> 1;
        self.flag_registers
            .set_flags((result == 0) as i8, 0, 0, carry as i8);
        self.set_z_value(z, result);
    }

    fn bit(&mut self, y: u8, z: u8) {
        let value_z = self.get_z_value(z);
        let bit = get_bit(value_z, y);
        self.flag_registers.set_flags(!bit as i8, 0, 1, -1);
    }

    fn res(&mut self, y: u8, z: u8) {
        let mut value_z = self.get_z_value(z);
        value_z = set_bit(value_z, y, false);
        self.set_z_value(z, value_z);
    }

    fn set(&mut self, y: u8, z: u8) {
        let mut value_z = self.get_z_value(z);
        value_z = set_bit(value_z, y, true);
        self.set_z_value(z, value_z);
    }
}
