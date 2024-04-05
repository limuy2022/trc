use crate::compiler;
use libcore::*;

pub fn compile(opt: compiler::CompileOption) -> RunResult<()> {
    let mut compiler = compiler::Compiler::new(opt);
    compiler.lex()?;
    Ok(())
}
