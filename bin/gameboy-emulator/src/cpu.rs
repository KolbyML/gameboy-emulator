use crate::{
    memory_bus::MemoryBus,
    registers::{FlagRegister, FlagRegisters, Register, Registers},
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
    // const ALU_TABLE: [fn(&mut CPU, u8); 8] = [
    //     CPU::add_a,
    //     CPU::adc_a,
    //     CPU::sub_a,
    //     CPU::sbc_a,
    //     CPU::and_a,
    //     CPU::xor_a,
    //     CPU::or_a,
    //     CPU::cp_a,
    // ];
    // const ROT_

    pub fn new() -> Self {
        CPU {
            registers: Registers::new(),
            flag_registers: FlagRegisters::new(),
            memory_bus: MemoryBus::new(),
        }
    }

    pub fn cpu_cycle(&mut self) {
        let opcode = self.memory_bus.read_byte(self.registers.program_counter);
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
            _ => todo!(),
        }
    }

    fn no_operation(&mut self) {}

    fn load_nn_sp(&mut self) {
        let lower_8bits = self
            .memory_bus
            .read_byte(self.registers.program_counter + 1);
        let upper_8bits = self
            .memory_bus
            .read_byte(self.registers.program_counter + 2);
        let u16 = (upper_8bits as u16) << 8 | lower_8bits as u16;
        self.memory_bus
            .write_word(u16, self.registers.stack_pointer);
    }

    /// stop doesn't have to be implemented
    fn stop(&mut self) {}

    fn relative_jump_d(&mut self) {
        let d = self
            .memory_bus
            .read_byte(self.registers.program_counter + 1) as i8;
        self.registers.program_counter = (self.registers.program_counter as i32 + d as i32) as u16;
    }

    fn relative_jump_cc_d(&mut self, y: u8) {
        let d = self
            .memory_bus
            .read_byte(self.registers.program_counter + 1) as i8;
        let cc = CPU::CC_TABLE[(d >> 4) as usize];
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
    }

    fn load_immediate_16bit(&mut self, p: u8) {
        let register = CPU::RP_TABLE[p as usize];
        let nn = self
            .memory_bus
            .read_word(self.registers.program_counter + 1);
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
        let register = CPU::R_TABLE[y as usize];
        if register.is_register() {
            self.registers.increment_register(register);
        } else {
            self.registers.increment_virtual_register(register);
        }
    }

    fn decrement_16bit(&mut self, p: u8) {
        let register = CPU::RP_TABLE[p as usize];
        self.registers.decrement_virtual_register(register);
    }

    fn decrement_8bit(&mut self, y: u8) {
        let register = CPU::R_TABLE[y as usize];
        if register.is_register() {
            self.registers.decrement_register(register);
        } else {
            self.registers.decrement_virtual_register(register);
        }
    }

    fn load_immediate_8bit(&mut self, y: u8) {
        let value = self
            .memory_bus
            .read_byte(self.registers.program_counter + 1);
        let register = CPU::R_TABLE[y as usize];
        if register.is_register() {
            self.registers.set_register(register, value);
        } else {
            self.registers.set_virtual_register(register, value as u16);
        }
    }

    fn rotate_left_circular_accumulator(&mut self) {
        self.flag_registers
            .set_flags(0, 0, 0, self.registers.get_bit(Register::A, 7) as i8);
        self.registers.rotate_left(Register::A);
    }

    fn rotate_right_circular_accumulator(&mut self) {
        self.flag_registers
            .set_flags(0, 0, 0, self.registers.get_bit(Register::A, 0) as i8);
        self.registers.rotate_right(Register::A);
    }

    fn rotate_left_accumulator(&mut self) {
        let carry = self.flag_registers.c;
        self.flag_registers
            .set_flags(0, 0, 0, self.registers.get_bit(Register::A, 7) as i8);
        self.registers.rotate_left(Register::A);
        self.registers.set_bit(Register::A, 0, carry);
    }

    fn rotate_right_accumulator(&mut self) {
        let carry = self.flag_registers.c;
        self.flag_registers
            .set_flags(0, 0, 0, self.registers.get_bit(Register::A, 0) as i8);
        self.registers.rotate_right(Register::A);
        self.registers.set_bit(Register::A, 7, carry);
    }

    fn decimal_adjust_accumulator(&mut self) {
        if self.flag_registers.n == 0 {
            if self.flag_registers.c != 0 || self.registers.a > 0x99 {
                self.registers.a += 0x60;
                self.flag_registers.c = 1;
            }
            if self.flag_registers.h != 0 || (self.registers.a & 0x0f) > 0x09 {
                self.registers.a += 0x6;
            }
        } else {
            if self.flag_registers.c != 0 {
                self.registers.a -= 0x60;
            }
            if self.flag_registers.h != 0 {
                self.registers.a -= 0x6;
            }
        }

        self.flag_registers.z = (self.registers.a == 0) as u8;
        self.flag_registers.h = 0;
    }

    fn complement_accumulator(&mut self) {
        self.flag_registers.set_flags(-1, -1, -1, -1);
        self.registers.a = !self.registers.a;
    }

    fn set_carry_flag(&mut self) {
        self.flag_registers.set_flags(-1, 0, 0, 1);
    }

    fn complement_carry_flag(&mut self) {
        self.flag_registers
            .set_flags(-1, 0, 0, (self.flag_registers.c ^ 1) as i8);
    }
}
