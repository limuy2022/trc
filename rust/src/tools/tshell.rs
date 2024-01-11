//! the read execute print loop for trc

use gettextrs::gettext;
use colored::*;
use std::io::{self, Write};

fn get_block() {

}

pub fn tshell() {
    println!("{}\n\n", gettext("Welcome to tshell.Type help() to get more infomation").bold());
    loop {
        print!("tshell>");
        io::stdout().flush().unwrap();
        let mut line = String::new();
        io::stdin().read_line(&mut line).unwrap();
    }
}
