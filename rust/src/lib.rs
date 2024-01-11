pub mod base;
pub mod compiler;
pub mod tools;
pub mod tvm;

use clap::{self, Subcommand, Parser};

#[derive(Debug, Parser)]
#[command(author="limuy", version="0.1", about, long_about = None)]
#[command(propagate_version = true)]
struct Args {
    #[command(subcommand)]
    mode: Commands,
    #[arg()]
    files: Vec<String>,
}

#[derive(Debug, Subcommand)]
enum Commands {
    build {
        optimize: bool
    },
    tshell {}
}

pub fn run() {
    let cli = Args::parse();
    match cli.mode {
        Commands::build { optimize:opt } => {
            for i in cli.files {
                tools::compile(compiler::Option::new(opt, i));
            }
        },
        Commands::tshell {  } => {
            tools::tshell::tshell();
        }
    };
}
