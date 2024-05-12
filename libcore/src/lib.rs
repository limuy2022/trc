#![deny(warnings)]
#![feature(register_tool)]
#![register_tool(tarpaulin)]
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
            .try_into()
            .unwrap()
    };
}
#[macro_export]
macro_rules! floatsz {
    () => {
        std::mem::size_of::<$crate::TrcFloatInternal>()
            .try_into()
            .unwrap()
    };
}
#[macro_export]
macro_rules! charsz {
    () => {
        std::mem::size_of::<$crate::TrcCharInternal>()
            .try_into()
            .unwrap()
    };
}
#[macro_export]
macro_rules! strsz {
    () => {
        std::mem::size_of::<$crate::TrcStrInternal>()
            .try_into()
            .unwrap()
    };
}
#[macro_export]
macro_rules! boolsz {
    () => {
        std::mem::size_of::<bool>().try_into().unwrap()
    };
}
#[macro_export]
macro_rules! objsz {
    () => {
        std::mem::size_of::<*mut dyn $crate::TrcObj>()
            .try_into()
            .unwrap()
    };
}
rust_i18n::i18n!("../locales");
