use crate::{base::error::RunResult, tvm::DynaData};

pub fn tvm_print(dydata: &mut DynaData) -> RunResult<()> {
    let obj = dydata.obj_stack.pop();
    print!("{}", obj.unwrap());
    Ok(())
}
