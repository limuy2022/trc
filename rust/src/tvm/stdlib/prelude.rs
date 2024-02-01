use crate::{base::error::RunResult, tvm::DynaData};

pub fn tvm_print(dydata: &mut DynaData) -> RunResult<()> {
    print!("win!");
    Ok(())
}
