pub mod tools;
pub mod compiler;
pub mod base;

use std::env::args;
use std::process::exit;

fn main() {
    let mode:String = match args().nth(1) {
        Some(tr) => tr,
        _ => {
            println!("A mode isn't given.");
            exit(0);
        },
    };
    if mode == "build" {
        tools::compile();
    } else if mode == "run" {

    }
}
