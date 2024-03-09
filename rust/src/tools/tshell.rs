//! the read execute print loop for trc

use colored::*;
use rust_i18n::t;
use std::io::{self, Write};

use crate::compiler;

fn get_block() {}

pub fn tshell() {
    println!("{}\n\n", t!("tshell.welcome").bold());
    let mut compiler = compiler::Compiler::new_string_compiler(
        compiler::Option::new(false, compiler::InputSource::StringInternal),
        "",
    );
    // let ast = compiler.get_ast_obj();
    loop {
        print!("tshell>");
        io::stdout().flush().unwrap();
        let mut line = String::new();
        io::stdin().read_line(&mut line).unwrap();
        let source = Box::new(compiler::StringSource::new(line));
        compiler.modify_input(source);
    }
}
