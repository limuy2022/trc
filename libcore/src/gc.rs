//! provide gc for trc

#[derive(Default, Debug)]
pub struct GcMgr {}

impl GcMgr {
    pub fn new() -> Self {
        Self {}
    }

    pub fn alloc<T>(&mut self, obj: T) -> *mut T {
        // unsafe { alloc(Layout::new::<T>()) as *mut T }
        Box::into_raw(Box::new(obj))
    }
}
