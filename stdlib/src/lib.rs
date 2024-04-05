use derive::def_module;
pub use libcore;
use libcore::libbasic::*;
use std::{collections::HashMap, sync::OnceLock};

pub mod algo;
pub mod ds;
pub mod math;
pub mod prelude;
pub mod sys;

rust_i18n::i18n!("locales");

def_module!(
    module_name = std,
    submodules = [prelude, ds, algo, math, sys]
);

/// 导入标准库
fn import_stdlib() -> (&'static Module, &'static ModuleStorage) {
    // this is for any type
    static STDLIB_STORAGE: OnceLock<ModuleStorage> = OnceLock::new();
    static RETURN_VAL: OnceLock<Module> = OnceLock::new();
    let stro = STDLIB_STORAGE.get_or_init(|| {
        let mut storage = ModuleStorage::new();
        // any
        RustClass::new_in_storage("any", HashMap::new(), None, None, &mut storage);
        RETURN_VAL.get_or_init(|| module_init(&mut storage));
        storage
    });
    (RETURN_VAL.get().unwrap(), stro)
}

pub fn get_stdlib() -> &'static Module {
    import_stdlib().0
}

pub fn get_storage() -> &'static ModuleStorage {
    import_stdlib().1
}

pub fn get_prelude_function(func_name: &str) -> Option<&'static RustFunction> {
    get_stdlib()
        .sub_modules
        .get("prelude")
        .unwrap()
        .functions
        .get(func_name)
}
