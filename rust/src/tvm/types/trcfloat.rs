use super::TrcObj;

pub struct TrcFloat {

}

impl TrcObj for TrcFloat {
    fn get_type_name(&self) -> &str {
        "float"
    }
}
