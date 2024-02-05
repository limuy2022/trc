pub mod base;
pub mod cfg;
pub mod compiler;
pub mod tools;
pub mod tvm;

use clap::{self, Parser, Subcommand};
use std::error::Error;

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
}

#[derive(Debug, Subcommand)]
enum Commands {
    Build {
        #[arg(short, long, default_value_t = false)]
        optimize: bool,
        #[arg()]
        files: Vec<String>,
    },
    Tshell {},
    Update {},
    Run {
        #[arg(short, long, default_value_t = false)]
        optimize: bool,
        #[arg()]
        files: Vec<String>,
    },
}

pub fn run() -> Result<(), Box<dyn Error>> {
    let cli = Args::parse();
    match cli.mode {
        Commands::Build {
            optimize: opt,
            files,
        } => {
            for i in files {
                tools::compile(compiler::Option::new(opt, compiler::InputSource::File(i)));
            }
        }
        Commands::Tshell {} => {
            tools::tshell::tshell();
        }
        Commands::Update {} => match tools::update::update() {
            Err(e) => {
                println!("{}", e);
            }
            Ok(_) => {}
        },
        Commands::Run {
            optimize: opt,
            files,
        } => {
            for i in files {
                match tools::run::run(compiler::Option::new(opt, compiler::InputSource::File(i))) {
                    Ok(_) => {}
                    Err(c) => {
                        eprintln!("{}", c);
                    }
                }
            }
        }
    };
    Ok(())
}
