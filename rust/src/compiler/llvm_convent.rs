use std::ptr::null_mut;

// use llvm_sys::core::{
//     LLVMAddFunction, LLVMContextCreate, LLVMCreateBuilderInContext, LLVMFunctionType,
//     LLVMInt32TypeInContext, LLVMModuleCreateWithName,
// };

use crate::base::codegen::StaticData;

pub fn convert(tvm_data: StaticData) {
    unsafe {
        // let context = LLVMContextCreate();
        // let module = LLVMModuleCreateWithName(b"main\0".as_ptr() as *const i8);
        // let builder = LLVMCreateBuilderInContext(context);
        // let i32_ty = LLVMInt32TypeInContext(context);
        // // 创建main函数
        // let main_ty = LLVMFunctionType(i32_ty, null_mut(), 0, 0);
        // let main_func = LLVMAddFunction(module, b"main\0".as_ptr() as *const _, main_ty);
    }
}
