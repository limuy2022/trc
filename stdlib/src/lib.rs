use collection_literals::collection;
use derive::{def_module, def_module_export, trc_class, trc_method};
pub use libcore;
use libcore::libbasic::*;
use std::collections::HashMap;

pub mod algo;
pub mod ds;
pub mod math;
pub mod prelude;
pub mod sys;

rust_i18n::i18n!("../locales");

#[trc_class]
struct TrcAny {}

impl TrcAny {
    fn override_export() -> HashMap<OverrideOperations, OverrideWrapper> {
        collection_literals::hash![]
    }
}

#[trc_method]
impl TrcAny {}

def_module!(
    module_name = std,
    classes = [TrcAny => any]
    submodules = [prelude, ds, algo, math, sys]
);
def_module_export!();
