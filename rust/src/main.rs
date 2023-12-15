pub mod tools;
pub mod compiler;

use std::io;
use rand::Rng;
use std::cmp::Ordering;
use std::env;
use std::env::args;

fn main() {
    let mode = args().nth(1).expect("a mode isn't given");
    if mode == "build" {
        tools::compile();
    } else if mode == "run" {

    }
}
