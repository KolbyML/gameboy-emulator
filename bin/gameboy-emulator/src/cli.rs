use std::path::PathBuf;

use clap::Parser;

#[derive(Debug, Parser)]
#[command(author, version, about, long_about = None)]
pub struct GameboyConfig {
    #[arg(short, long, default_value = "false")]
    pub debug: bool,

    #[arg()]
    pub rom: PathBuf,
}
