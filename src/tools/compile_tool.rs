use crate::compiler;
use std::process::exit;

pub fn compile(opt: compiler::Option, dev: bool) {
    let mut compiler = compiler::Compiler::new(opt);
    let tmp = compiler.lex();
    match tmp {
        Ok(data) => {}
        Err(e) => {
            if dev {
                Err::<(), _>(e).unwrap();
            } else {
                eprintln!("{}", e);
                exit(1)
            }
        }
    }
}
