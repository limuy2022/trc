use crate::{
    base::error::RunResult,
    compiler::{self, Compiler},
    tvm::Vm,
};

pub fn run(opt: compiler::Option) -> RunResult<()> {
    let mut compiler = Compiler::new(opt);
    let static_data = compiler.lex()?;
    let mut run_vm = Vm::new();
    run_vm.set_static_data(static_data);
    run_vm.run()?;
    Ok(())
}
