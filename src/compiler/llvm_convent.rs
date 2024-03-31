use crate::base::codegen::StaticData;

// use llvm_sys::core::{
//     LLVMAddFunction, LLVMContextCreate, LLVMCreateBuilderInContext, LLVMFunctionType,
//     LLVMInt32TypeInContext, LLVMModuleCreateWithName,
// };

pub fn convert(_tvm_data: StaticData) {
    // let context = LLVMContextCreate();
    // let module = LLVMModuleCreateWithName(b"main\0".as_ptr() as *const i8);
    // let builder = LLVMCreateBuilderInContext(context);
    // let i32_ty = LLVMInt32TypeInContext(context);
    // // 创建main函数
    // let main_ty = LLVMFunctionType(i32_ty, null_mut(), 0, 0);
    // let main_func = LLVMAddFunction(module, b"main\0".as_ptr() as *const _, main_ty);
}
