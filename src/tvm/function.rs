/// A context structure which hold the running info of the function
use super::Byte;

pub struct Frame {
    pub prev_addr: usize,
    pub var_table_addr: *mut Byte,
}

impl Frame {
    pub fn new(prev_addr: usize, var_table_addr: *mut Byte) -> Self {
        Self {
            prev_addr,
            var_table_addr,
        }
    }

    pub fn set_var<T: 'static>(&mut self, addr: usize, data: T) {
        unsafe {
            *(self.var_table_addr.byte_offset(addr as isize) as *mut Byte as *mut T) = data;
        }
    }

    pub fn get_var<T: Copy + 'static>(&self, addr: usize) -> T {
        unsafe { *(self.var_table_addr.byte_offset(addr as isize) as *const Byte as *const T) }
    }
}
