use super::TrcObj;
pub struct TrcBool {

}

impl TrcObj for TrcBool {
    fn get_type_name(&self) -> &str {
        "bool"
    }
}
