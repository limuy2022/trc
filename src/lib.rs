pub mod base;
pub mod cfg;
pub mod compiler;
pub mod tools;
pub mod tvm;

use clap::{Parser, Subcommand};
use std::error::Error;

rust_i18n::i18n!("locales");
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

macro_rules! make_commands {
    ($struct_name:ident, $($repeat_compile_command:ident, )* | $($other_command: tt)*) => {
        #[derive(Debug, Subcommand)]
        enum $struct_name {
            $(
            $repeat_compile_command {
                #[arg(short, long, default_value_t = false)]
                optimize: bool,
                #[arg()]
                files: Vec<String>,
            },
            )*
            $(
                $other_command
            )*
        }
    };
}

make_commands!(Commands, Build, Run, Dis, | Tshell {}, Update{});

pub fn run() -> Result<(), Box<dyn Error>> {
    let cli = Args::parse();
    match cli.mode {
        Commands::Build { optimize, files } => {
            for i in files {
                match tools::compile(compiler::Option::new(
                    optimize,
                    compiler::InputSource::File(i),
                )) {
                    Ok(_) => {}
                    Err(c) => {
                        eprintln!("{}", c);
                    }
                };
            }
        }
        Commands::Tshell {} => {
            tools::tshell::tshell()?;
        }
        Commands::Update {} => {
            if let Err(e) = tools::update::update() {
                println!("{}", e);
            }
        }
        Commands::Run { optimize, files } => {
            for i in files {
                match tools::run::run(compiler::Option::new(
                    optimize,
                    compiler::InputSource::File(i),
                )) {
                    Ok(_) => {}
                    Err(c) => {
                        eprintln!("{}", c);
                    }
                }
            }
        }
        Commands::Dis { files, optimize } => {
            for i in files {
                match tools::dis::dis(compiler::Option::new(
                    optimize,
                    compiler::InputSource::File(i),
                )) {
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
