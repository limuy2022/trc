use crate::compiler;
use libcore::*;

pub fn build(opt: compiler::CompileOption) -> RuntimeResult<()> {
    let mut compiler = compiler::Compiler::new(opt);
    compiler.lex()?;
    Ok(())
}
