use libcore::{Module, ModuleStorage};

pub type GetLibFuncTy = unsafe fn() -> &'static Module;
pub type GetStorageFuncTy = unsafe fn() -> &'static ModuleStorage;

pub fn load_module_storage(lib: &libloading::Library) -> (&'static Module, &'static ModuleStorage) {
    let get_lib_func: libloading::Symbol<GetLibFuncTy> =
        unsafe { lib.get(libcore::GET_LIB_FUNC_NAME.as_bytes()).unwrap() };
    let get_storage_func: libloading::Symbol<GetStorageFuncTy> =
        unsafe { lib.get(libcore::GET_STORAGE_FUNC_NAME.as_bytes()).unwrap() };
    unsafe { (get_lib_func(), get_storage_func()) }
}
