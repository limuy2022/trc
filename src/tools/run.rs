use crate::{
    compiler::{self, Compiler},
    tvm::Vm,
};
use libcore::*;

pub fn run(opt: compiler::CompileOption) -> RuntimeResult<()> {
    let mut compiler = Compiler::new(opt);
    let tmp = compiler.lex()?;
    // let tmp = static_data.return_static_data();
    let mut run_vm = Vm::new(&tmp);
    run_vm.run()?;
    Ok(())
}
