use std::process::exit;

use crate::compiler;

pub fn compile(opt: compiler::Option) {
    let mut compiler = compiler::Compiler::new(opt);
    match compiler.lex() {
        Ok(data) => {}
        Err(e) => {
            eprintln!("{}", e);
            exit(1)
        }
    }
}
