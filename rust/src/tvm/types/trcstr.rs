use super::TrcObj;

pub struct TrcStr {

}

impl TrcObj for TrcStr {
    fn get_type_name(&self) -> &str {
        "str"
    }
}
