use crate::compiler;

pub fn compile(opt: compiler::Option) {
    let mut compiler = compiler::Compiler::new(opt);
    compiler.lex();
}
