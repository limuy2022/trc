use std::fmt::Display;

use super::TrcObj;

pub struct TrcFloat {
    pub value:f64
}

impl TrcFloat {
    pub fn new(value: f64) -> TrcFloat {
        Self {
            value
        }
    }
}

impl TrcObj for TrcFloat {
    fn get_type_name(&self) -> &str {
        "float"
    }
}

impl Display for TrcFloat {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", self.value)   
    }
}
