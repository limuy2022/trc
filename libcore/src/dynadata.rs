use std::{mem::size_of, sync::OnceLock};

use crate::gc::GcMgr;

pub type Byte = u64;

pub fn get_max_stack_sz() -> usize {
    static T: OnceLock<usize> = OnceLock::new();
    *T.get_or_init(|| 1024 * 1024 * 2 / size_of::<Byte>())
}

#[derive(Default)]
pub struct DynaData {
    pub gc: GcMgr,
    run_stack: Vec<Byte>,
    pub var_store: Vec<Byte>,
    stack_ptr: usize,
    // 变量已经使用的内存空间大小
    var_used: usize,
    #[cfg(debug_assertions)]
    type_used: Vec<&'static str>,
}

impl DynaData {
    pub fn new() -> Self {
        let mut ret = Self {
            run_stack: Vec::with_capacity(get_max_stack_sz()),
            var_store: Vec::with_capacity(get_max_stack_sz()),
            stack_ptr: 0,
            ..Default::default()
        };
        unsafe {
            ret.run_stack.set_len(get_max_stack_sz());
            ret.var_store.set_len(get_max_stack_sz());
        }
        ret
    }

    pub fn init_global_var_store(&mut self, cap: usize) {
        self.var_used = cap;
        if self.var_store.len() > cap {
            return;
        }
        self.var_store.resize(cap, Byte::default());
    }

    /// Push data of this [`DynaData`].
    pub fn push_data<T: 'static>(&mut self, data: T) {
        unsafe {
            (self
                .run_stack
                .as_mut_ptr()
                .byte_offset(self.stack_ptr as isize) as *mut T)
                .write(data);
        }
        self.stack_ptr += size_of::<T>();
        #[cfg(debug_assertions)]
        {
            self.type_used.push(std::any::type_name::<T>());
        }
    }

    /// Pop data of the data stack
    ///
    /// # Panics
    ///
    /// Panics if in debug mode and the type of `T` is wrong.
    pub fn pop_data<T: Copy + 'static>(&mut self) -> T {
        let sz = size_of::<T>();
        #[cfg(debug_assertions)]
        {
            let info = std::any::type_name::<T>();
            let info_stack = self.type_used.pop().unwrap();
            if info_stack != info {
                panic!(
                    "pop data type error.Expected get {}.Actually has {}",
                    std::any::type_name::<T>(),
                    info_stack
                );
            }
            debug_assert!(self.stack_ptr >= sz);
        }
        self.stack_ptr -= sz;
        unsafe { *(self.run_stack.as_ptr().byte_offset(self.stack_ptr as isize) as *const T) }
    }

    /// Pop n bytes of stack
    pub fn pop_n_bytes_data(&mut self, n: usize) -> *mut Byte {
        debug_assert!(self.stack_ptr >= n);
        self.stack_ptr -= n;
        unsafe {
            let ret = self
                .run_stack
                .as_mut_ptr()
                .byte_offset(self.stack_ptr as isize);
            #[cfg(debug_assertions)]
            {
                self.type_used.pop().unwrap();
            }
            ret
        }
    }

    /// Returns the top data of the data stack.
    ///
    /// # Panics
    ///
    /// Panics if .
    pub fn read_top_data<T: Copy + 'static>(&self) -> T {
        let sz = size_of::<T>();
        #[cfg(debug_assertions)]
        {
            let info = std::any::type_name::<T>();
            let info_stack = self.type_used.last().unwrap();
            if *info_stack != info {
                panic!(
                    "pop data type error.Expected get {}.Actually has {}",
                    std::any::type_name::<T>(),
                    info_stack
                );
            }
            debug_assert!(self.stack_ptr >= sz);
        }
        unsafe {
            *(self
                .run_stack
                .as_ptr()
                .byte_offset((self.stack_ptr - sz) as isize) as *const T)
        }
    }

    /// Sets the var of this [`DynaData`].
    ///
    /// # Safety
    /// make sure your addr is valid, or it will panic
    /// .
    pub unsafe fn set_var<T: 'static>(&mut self, addr: usize, data: T) {
        unsafe {
            (self.get_var_addr_mut(addr) as *mut T).write(data);
        }
    }

    /// Sets the var of this [`DynaData`].
    ///
    /// # Safety
    /// make sure your addr is valid, or it will panic
    /// .
    pub unsafe fn get_var<T: Copy + 'static>(&self, addr: usize) -> T {
        debug_assert!(addr < self.var_used);
        unsafe { *(self.get_var_addr(addr) as *const T) }
    }

    /// Sets the var of this [`DynaData`].
    ///
    /// # Safety
    /// make sure your addr and src are valid, or it will panic
    /// .
    pub unsafe fn write_to_val(&mut self, addr: usize, src: *mut Byte, n: usize) {
        unsafe { self.get_var_addr_mut(addr).copy_from(src, n) }
    }

    fn get_var_addr(&self, addr: usize) -> *const Byte {
        unsafe { self.var_store.as_ptr().byte_offset(addr as isize) }
    }

    fn get_var_addr_mut(&mut self, addr: usize) -> *mut Byte {
        unsafe { self.var_store.as_mut_ptr().byte_offset(addr as isize) }
    }

    pub fn alloc_var_space(&mut self, need_sz: usize) -> *mut Byte {
        self.var_used += need_sz;
        if self.var_used > self.var_store.len() {
            self.var_store.resize(self.var_used, Byte::default());
        }
        unsafe {
            self.var_store
                .as_mut_ptr()
                .byte_offset((self.var_used - need_sz) as isize)
        }
    }

    pub fn dealloc_var_space(&mut self, need_sz: usize) {
        self.var_used -= need_sz;
    }

    pub fn get_var_used(&self) -> usize {
        self.var_used
    }

    pub fn get_gc(&mut self) -> &mut GcMgr {
        &mut self.gc
    }
}
