pub mod base;
pub mod compiler;
pub mod tools;
pub mod tvm;

use std::env::args;
use std::process::exit;

fn main() -> Result<(), Box<dyn std::error::Error>> {
    gettextrs::setlocale(gettextrs::LocaleCategory::LcAll, "");
    gettextrs::bindtextdomain("trans", "locales")?;
    gettextrs::textdomain("trans")?;
    let mode: String = match args().nth(1) {
        Some(tr) => tr,
        _ => {
            println!("A mode isn't given.");
            exit(0);
        }
    };
    if mode == "build" {
        tools::compile();
    } else if mode == "run" {
    }
    Ok(())
}
