//! some constant values and configurations in trc

use lazy_static::lazy_static;

pub const MAIN_MODULE_NAME: &str = "main";
pub const FLOAT_OVER_FLOW_LIMIT: usize = 18;
pub const VERSION: &str = "0.1.0";
lazy_static! {
    pub static ref VERSION_DESTRUCT: (u8, u8, u8) = (0, 1, 0);
}
