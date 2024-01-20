use std::fmt::Display;

use super::TrcObj;
pub struct TrcBool {
    pub value: bool,
}

impl TrcObj for TrcBool {
    fn get_type_name(&self) -> &str {
        "bool"
    }


}

impl Display for TrcBool {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        if self.value {
            write!(f, "True")
        } else {
            write!(f, "False")
        }
    }
}

impl TrcBool {
    pub fn new(value: bool) -> TrcBool {
        Self {
            value
        }
    }
}
