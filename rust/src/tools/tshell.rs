//! the read execute print loop for trc

use colored::*;
use rust_i18n::t;
use std::io::{self, Write};

fn get_block() {}

pub fn tshell() {
    println!("{}\n\n", t!("tshell.welcome").bold());
    loop {
        print!("tshell>");
        io::stdout().flush().unwrap();
        let mut line = String::new();
        io::stdin().read_line(&mut line).unwrap();
    }
}
