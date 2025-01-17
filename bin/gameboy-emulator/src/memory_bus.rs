pub struct MemoryBus {
    pub memory: [u8; 0x10000],
}

impl MemoryBus {
    pub fn new() -> Self {
        MemoryBus {
            memory: [0; 0x10000],
        }
    }

    pub fn read_byte(&self, address: u16) -> u8 {
        self.memory[address as usize]
    }

    pub fn write_byte(&mut self, address: u16, value: u8) {
        self.memory[address as usize] = value;
    }

    pub fn read_word(&self, address: u16) -> u16 {
        let low_byte = self.read_byte(address) as u16;
        let high_byte = self.read_byte(address + 1) as u16;
        (high_byte << 8) | low_byte
    }

    pub fn write_word(&mut self, address: u16, value: u16) {
        let low_byte = value as u8;
        let high_byte = (value >> 8) as u8;
        self.write_byte(address, low_byte);
        self.write_byte(address + 1, high_byte);
    }
}
