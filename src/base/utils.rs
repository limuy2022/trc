use std::ops::DivAssign;

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
