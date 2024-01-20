use std::fmt::Display;

use super::TrcObj;

pub struct TrcStr {
    value: String
}

impl TrcObj for TrcStr {
    fn get_type_name(&self) -> &str {
        "str"
    }
}

impl Display for TrcStr {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", self.value)   
    }
}
