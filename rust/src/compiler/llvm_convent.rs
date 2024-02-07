use llvm_sys::core::{LLVMContextCreate, LLVMCreateBuilderInContext, LLVMModuleCreateWithName};

use crate::base::codegen::StaticData;

pub fn convert(tvm_data: StaticData) {
    unsafe {
        let context = LLVMContextCreate();
        let module = LLVMModuleCreateWithName(b"main\0".as_ptr() as *const i8);
        let builder = LLVMCreateBuilderInContext(context);
    }
}
