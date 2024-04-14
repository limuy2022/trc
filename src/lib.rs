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
    version = crate::cfg::VERSION,
    about = "A powerful programming language",
    long_about = "This programming language is powerful, effective, safe, easy-to-learn"
)]
#[command(propagate_version = true)]
struct Args {
    #[command(subcommand)]
    mode: Commands,
}

macro_rules! make_commands {
    ($struct_name:ident, $($repeat_compile_command:ident {$($commands_compile_other: tt)*}, )* | $($other_command: tt)*) => {
        #[derive(Debug, Subcommand)]
        enum $struct_name {
            $(
            $repeat_compile_command {
                #[arg(short, long, default_value_t = false)]
                optimize: bool,
                #[arg()]
                files: Vec<String>,
                $($commands_compile_other)*
            },
            )*
            $(
                $other_command
            )*
        }
    };
}

make_commands!(Commands, Build {}, Run {}, Dis {
    #[arg(short, long, default_value_t = false)]
    rustcode: bool
}, | Tshell {}, Version {});

shadow_rs::shadow!(build);

pub fn run() -> Result<(), Box<dyn Error>> {
    let cli = Args::parse();
    match cli.mode {
        Commands::Build { optimize, files } => {
            for i in files {
                match tools::build(compiler::CompileOption::new(
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
            tools::tshell()?;
        }
        Commands::Run { optimize, files } => {
            for i in files {
                match tools::run(compiler::CompileOption::new(
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
        Commands::Dis {
            files,
            optimize,
            rustcode,
        } => {
            for i in files {
                match tools::dis(
                    compiler::CompileOption::new(optimize, compiler::InputSource::File(i)),
                    rustcode,
                ) {
                    Ok(_) => {}
                    Err(c) => {
                        eprintln!("{}", c);
                    }
                }
            }
        }
        Commands::Version {} => {
            println!(
                "trc version {} {} {}\nbuild by {} at {} ",
                cfg::VERSION,
                shadow_rs::branch(),
                build::COMMIT_HASH,
                build::RUST_VERSION,
                build::BUILD_TIME
            );
        }
    };
    Ok(())
}
