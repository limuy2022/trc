use crate::{base::error::RunResult, compiler};

pub fn compile(opt: compiler::Option) -> RunResult<()> {
    let mut compiler = compiler::Compiler::new(opt);
    compiler.lex()?;
    Ok(())
}
