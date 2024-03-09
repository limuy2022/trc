//! the read execute print loop for trc

use colored::*;
use rust_i18n::t;
use std::io::{self, Write};

use crate::{
    base::{codegen::StaticData, error::RunResult},
    compiler,
    tvm::Vm,
};

fn get_block() -> String {
    let mut block = String::new();
    let mut cnt = 1;
    loop {
        for i in 0..cnt {
            print!("    ");
        }
        print!("...");
        io::stdout().flush().unwrap();
        let mut line = String::new();
        io::stdin().read_line(&mut line).unwrap();
        block += &line;
        if line.ends_with("{\n") {
            cnt += 1;
        }
        if line.ends_with("}\n") {
            cnt -= 1;
            if cnt == 0 {
                break;
            }
        }
    }
    block
}

pub fn tshell() -> RunResult<()> {
    println!("{}\n\n", t!("tshell.welcome").bold());
    let mut compiler = compiler::Compiler::new_string_compiler(
        compiler::Option::new(false, compiler::InputSource::StringInternal),
        "",
    );
    let mut ast = compiler.lex()?;
    let mut vm = unsafe { Vm::new(&*(ast.prepare_get_static() as *const StaticData)) };
    loop {
        print!("tshell>");
        io::stdout().flush().unwrap();
        let mut line = String::new();
        io::stdin().read_line(&mut line).unwrap();
        if line.ends_with("{\n") {
            line += &get_block();
        }
        let source = Box::new(compiler::StringSource::new(line));
        ast.token_lexer.modify_input(source);
        ast.clear_inst();
        ast.token_lexer.clear_error();
        ast.generate_code().unwrap_or_else(|e| {
            eprintln!("{}", e);
        });
        vm.set_static_data(unsafe { &*(ast.prepare_get_static() as *const StaticData) });
        match vm.run() {
            Ok(_) => {}
            Err(e) => {
                eprintln!("{}", e);
            }
        }
    }
}

mod tests {}
