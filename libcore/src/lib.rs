pub mod codegen;
pub mod dynadata;
pub mod error;
pub mod gc;
pub mod libbasic;
pub mod types;

pub use codegen::*;
pub use dynadata::*;
pub use error::*;
pub use libbasic::*;
pub use types::*;

pub const GET_LIB_FUNC_NAME: &str = "get_lib";
pub const GET_STORAGE_FUNC_NAME: &str = "get_storage";

#[macro_export]
macro_rules! intsz {
    () => {
        std::mem::size_of::<$crate::TrcIntInternal>()
    };
}
#[macro_export]
macro_rules! floatsz {
    () => {
        std::mem::size_of::<$crate::TrcFloatInternal>()
    };
}
#[macro_export]
macro_rules! charsz {
    () => {
        std::mem::size_of::<$crate::TrcCharInternal>()
    };
}
#[macro_export]
macro_rules! strsz {
    () => {
        std::mem::size_of::<$crate::TrcStrInternal>()
    };
}
#[macro_export]
macro_rules! boolsz {
    () => {
        std::mem::size_of::<bool>()
    };
}
#[macro_export]
macro_rules! objsz {
    () => {
        std::mem::size_of::<*mut dyn $crate::TrcObj>()
    };
}
rust_i18n::i18n!("../locales");
