//! provide gc for trc

#[derive(Default, Debug)]
pub struct GcMgr {
    objs: Vec<*mut ()>,
}

impl GcMgr {
    pub fn new() -> Self {
        Self { objs: Vec::new() }
    }

    pub fn alloc<T>(&mut self, obj: T) -> *mut T {
        // unsafe { alloc(Layout::new::<T>()) as *mut T }
        // TODO: change it into alloc and change = into write
        Box::into_raw(Box::new(obj))
    }
}
