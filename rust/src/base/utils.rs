use std::ops::DivAssign;

#[macro_export]
macro_rules! hash_map {
    [$($key:expr => $val:expr),*] => {
        {
            use std::collections::hash_map::HashMap;
            let mut ret = HashMap::new();
            $(
                ret.insert($key, $val);
            )*
            ret
        }
    };
}

pub fn get_bit_num<T: PartialEq<i64> + DivAssign<i64>>(mut val: T) -> usize {
    if val == 0 {
        return 1;
    }
    let mut ret = 0;
    while val != 0 {
        val /= 10;
        ret += 1;
    }
    ret
}
