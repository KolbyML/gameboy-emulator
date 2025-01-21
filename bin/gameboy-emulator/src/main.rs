use clap::Parser;
use gameboy_emulator::{
    cli::GameboyConfig,
    cpu::CPU,
    memory_bus::MemoryBus,
    utils::{get_bit, set_bit},
};
use tracing::info;
use tracing_subscriber::EnvFilter;

fn main() -> anyhow::Result<()> {
    // Set the default log level to `info` if not set
    if std::env::var("RUST_LOG").is_err() {
        std::env::set_var("RUST_LOG", "info");
    }

    tracing_subscriber::fmt()
        .with_env_filter(EnvFilter::from_default_env())
        .init();

    let gameboy_config = GameboyConfig::parse();
    let mut cpu = CPU::new();
    cpu.memory_bus.load_rom(gameboy_config.rom)?;

    //     if (bus.get_ime() && bus.get_count()) {
    //         auto call = [](memory_bus & bus, uint8_t address) {
    //             bus.set_ime_false();
    //             registers.stack_pointer -= 1;
    //             bus.set_memory(registers.stack_pointer, ((registers.program_counter) & 0xFF00) >> 8);
    //             registers.stack_pointer -= 1;
    //             bus.set_memory(registers.stack_pointer, ((registers.program_counter) & 0x00FF));
    //             registers.program_counter = address;
    //         };
    //         uint8_t IE = bus.get_memory(0xFFFF);
    //         uint8_t IF = bus.get_memory(0xFF0F);

    //         if (get_bit(IE, 0) && get_bit(IF, 0)) {
    //             bus.set_memory(0xFF0F, set_bit2(IF, 0, 0));
    //             call(bus, 0x40);
    //         } else if (get_bit(IE, 1) && get_bit(IF, 1)) {
    //             bus.set_memory(0xFF0F, set_bit2(IF, 1, 0));
    //             call(bus, 0x48);
    //         } else if (get_bit(IE, 2) && get_bit(IF, 2)) {
    //             bus.set_memory(0xFF0F, set_bit2(IF, 2, 0));
    //             call(bus, 0x50);
    //         } else if (get_bit(IE, 3) && get_bit(IF, 3)) {
    //             bus.set_memory(0xFF0F, set_bit2(IF, 3, 0));
    //             call(bus, 0x58);
    //         } else if (get_bit(IE, 4) && get_bit(IF, 4)) {
    //             bus.set_memory(0xFF0F, set_bit2(IF, 4, 0));
    //             call(bus, 0x60);
    //         }
    //     }
    //     bus.inc_count();
    //     uint8_t IE = bus.get_memory(0xFFFF);
    //     uint8_t IF = bus.get_memory(0xFF0F);
    //     uint8_t op_code = bus.get_memory(registers.program_counter);

    //     if (op_code != 0x76) {
    //         cpu_cycle(bus, flag_register);
    //         bus.increment_timer(bus.get_cycles());
    //     } else {
    //         if ((IE & IF) != 0) {
    //             registers.program_counter++;
    //         }

    //         bus.increment_timer(4);
    //     }
    //     ppu.draw();

    //     //    blarggs test - serial output

    //     if (true && bus.get_memory(0xff02) == 0x81) {
    //         char c = bus.get_memory(0xff01);
    //         printf("%c", c);
    //         bus.set_memory(0xff02, 0x0);
    //     }
    // }

    loop {
        if cpu.memory_bus.ime && (cpu.memory_bus.count != 0) {
            let mut call = |bus: &mut MemoryBus, address: u16| {
                bus.ime = false;
                bus.write_word(cpu.registers.stack_pointer, cpu.registers.program_counter);
                cpu.registers.program_counter = address;
            };

            let IE = cpu.memory_bus.read_byte(0xFFFF);
            let IF = cpu.memory_bus.read_byte(0xFF0F);

            if get_bit(IE, 0) && get_bit(IF, 0) {
                cpu.memory_bus.write_byte(0xFF0F, set_bit(IF, 0, false));
                call(&mut cpu.memory_bus, 0x40);
            } else if get_bit(IE, 1) && get_bit(IF, 1) {
                cpu.memory_bus.write_byte(0xFF0F, set_bit(IF, 1, false));
                call(&mut cpu.memory_bus, 0x48);
            } else if get_bit(IE, 2) && get_bit(IF, 2) {
                cpu.memory_bus.write_byte(0xFF0F, set_bit(IF, 2, false));
                call(&mut cpu.memory_bus, 0x50);
            } else if get_bit(IE, 3) && get_bit(IF, 3) {
                cpu.memory_bus.write_byte(0xFF0F, set_bit(IF, 3, false));
                call(&mut cpu.memory_bus, 0x58);
            } else if get_bit(IE, 4) && get_bit(IF, 4) {
                cpu.memory_bus.write_byte(0xFF0F, set_bit(IF, 4, false));
                call(&mut cpu.memory_bus, 0x60);
            }
        }

        // cpu.memory_bus.count += 1;
        cpu.cpu_cycle();

        //    blarggs test - serial output
        if true && cpu.memory_bus.read_byte(0xFF02) == 0x81 {
            let c = cpu.memory_bus.read_byte(0xFF01);
            print!("{}", c as char);
            cpu.memory_bus.write_byte(0xFF02, 0x0);
        }
    }

    Ok(())
}
