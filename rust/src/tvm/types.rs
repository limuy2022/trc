mod data_structure;
mod trcfloat;
mod trcint;
mod trcstr;

pub trait TrcObj {
    fn output(&self) {

    }

    fn add(&self, _: Box<dyn TrcObj>) -> Box<dyn TrcObj> {
        panic!("Un");
    }
}
