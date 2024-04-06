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

rust_i18n::i18n!("locales");
