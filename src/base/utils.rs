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

pub struct PathType {
    pub is_file: bool,
    pub is_dir: bool,
}

impl PathType {
    pub fn new(is_file: bool, is_dir: bool) -> Self {
        Self { is_file, is_dir }
    }
}

pub fn check_path_type(path: &str) -> PathType {
    let mut ret = (false, false);
    if let Ok(metadata) = std::fs::metadata(path) {
        ret.0 = metadata.is_file();
        ret.1 = metadata.is_dir();
    }
    PathType::new(ret.0, ret.1)
}

pub fn get_next_check_char(
    iter: &mut impl Iterator<Item = char>,
    mut check: impl FnMut(char) -> bool,
) -> Option<char> {
    loop {
        let c = match iter.next() {
            Some(c) => c,
            None => return None,
        };
        if check(c) {
            continue;
        }
        return Some(c);
    }
}
