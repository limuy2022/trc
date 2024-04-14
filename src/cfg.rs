//! some constant values and configurations in trc

use crate::build::{self};
use std::{path::PathBuf, sync::OnceLock};

pub const MAIN_MODULE_NAME: &str = "main";
pub const FLOAT_OVER_FLOW_LIMIT: usize = 18;
pub const VERSION: &str = build::PKG_VERSION;
pub const BUILD_DIR_NAME: &str = "trcbuild";

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

pub fn get_version_destruct() -> &'static (u8, u8, u8) {
    static VERSION_SPLIT: OnceLock<(u8, u8, u8)> = OnceLock::new();
    VERSION_SPLIT.get_or_init(|| {
        let tmp: Vec<u8> = VERSION
            .split('.')
            .map(|s| s.parse::<u8>().unwrap())
            .collect();
        match tmp.as_slice() {
            [first, second, third] => (*first, *second, *third),
            _ => panic!("version is not right"),
        }
    })
}
