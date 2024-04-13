//! some constant values and configurations in trc

use std::{path::PathBuf, sync::OnceLock};

pub const MAIN_MODULE_NAME: &str = "main";
pub const FLOAT_OVER_FLOW_LIMIT: usize = 18;
pub const VERSION: &str = "0.0.1";
pub static VERSION_DESTRUCT: (u8, u8, u8) = (0, 0, 1);

pub fn get_history_file() -> &'static Option<PathBuf> {
    static PATH: OnceLock<Option<PathBuf>> = OnceLock::new();
    PATH.get_or_init(|| {
        let mut config_file = match dirs::config_dir() {
            Some(dir) => dir,
            None => return None,
        };
        config_file.push("trc");
        config_file.push(".trc_history");
        debug_assert!(config_file
            .as_os_str()
            .to_str()
            .unwrap()
            .ends_with("/trc/.trc_history"));
        Some(config_file)
    })
}
