use std::collections::HashMap;

pub fn kmp(main_string: &str, pattern: &str) -> usize {
    // 首先对模式串构建next数组
    let next_arr = kmp_next(pattern);
    let mut j: i64 = -1;
    // 我也不想把字符串先转换出来，但是必须要这么做
    let pattern: Vec<char> = pattern.chars().collect();
    let mut ans = 0;
    for i in main_string.chars() {
        while j != -1 && pattern[(j + 1) as usize] == i {
            j = next_arr[j as usize] as i64;
        }
        if pattern[(j + 1) as usize] == i {
            j += 1;
        }
        if j as usize == pattern.len() - 1 {
            ans += 1;
        }
    }
    ans
}

pub fn kmp_next(pattern: &str) -> Vec<i64> {
    let mut j: i64 = -1;
    let mut ret: Vec<i64> = vec![-1];
    let pattern: Vec<char> = pattern.chars().collect();
    // 从1开始匹配是因为第零个不需要匹配
    for i in 1..pattern.len() {
        while j != -1 && pattern[(j + 1) as usize] != pattern[i] {
            j = ret[j as usize] as i64;
        }
        if pattern[(j + 1) as usize] == pattern[i] {
            j += 1;
        }
        ret.push(j);
    }
    ret
}

pub fn sa(s: &str) {
    let mut sa: Vec<i32> = Vec::new();
    let mut t: HashMap<char, u32> = HashMap::new();
    for i in s.chars() {
        let tmp = t.entry(i).or_insert(0);
        *tmp += 1;
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn sa_1() {
        let s = "dkodkoe";
        let sarray = sa(s);
    }

    #[test]
    fn kmp_1() {
        assert_eq!(kmp("ABABABC", "ABA"), 2);
        assert_eq!(
            kmp(
                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            ),
            16
        );
        assert_eq!(
            kmp(
                "asdfasfababdababaadfasababdababagsdgababdababa",
                "ababdababa"
            ),
            3
        );
    }

    #[test]
    fn kmp_next_1() {
        assert_eq!(kmp_next("ABA"), vec![-1, -1, 0]);
    }
}
