use crate::{
    base::error::RunResult,
    compiler::{self, Compiler},
    tvm::Vm,
};

pub fn run(opt: compiler::Option) -> RunResult<()> {
    let mut compiler = Compiler::new(opt);
    let static_data = compiler.lex()?;
    let tmp = static_data.return_static_data();
    let mut run_vm = Vm::new(&tmp);
    run_vm.run()?;
    Ok(())
}
