use libcore::*;
use std::ptr::null_mut;

#[test]
fn test_dyna_data() {
    // panic!("deojfoejfoe");
    let mut data = DynaData::new();
    data.alloc_var_space(10);
    assert_eq!(data.get_var_used(), 10);
    data.dealloc_var_space(10);
    assert_eq!(data.get_var_used(), 0);
    data.push_data(10i64);
    data.push_data(20.0f64);
    data.push_data(30i64);
    data.push_data(null_mut::<i32>());
    data.push_data(50);
    data.push_data(false);
    // assert_eq == false
    assert!(!data.pop_data::<bool>());
    assert_eq!(data.pop_data::<i32>(), 50);
    assert_eq!(data.pop_data::<*mut i32>(), null_mut::<i32>());
    assert_eq!(data.pop_data::<i64>(), 30);
    assert_eq!(data.pop_data::<f64>(), 20.0);
    assert_eq!(data.pop_data::<i64>(), 10);
}
