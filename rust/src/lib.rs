pub mod base;
pub mod cfg;
pub mod compiler;
pub mod tools;
pub mod tvm;

use clap::{self, Parser, Subcommand};

#[derive(Debug, Parser)]
#[command(
    author = "limuy",
    version = "0.1",
    about = "A powerful programming language",
    long_about = "This programming language is powerful, effective, safe, easy-to-learn"
)]
#[command(propagate_version = true)]
struct Args {
    #[command(subcommand)]
    mode: Commands,
    #[arg()]
    files: Vec<String>,
}

#[derive(Debug, Subcommand)]
enum Commands {
    Build { optimize: bool },
    Tshell {},
}

pub fn run() {
    let cli = Args::parse();
    match cli.mode {
        Commands::Build { optimize: opt } => {
            for i in cli.files {
                tools::compile(compiler::Option::new(opt, compiler::InputSource::File(i)));
            }
        }
        Commands::Tshell {} => {
            tools::tshell::tshell();
        }
    };
}
