
pub fn kmp(main_string:&str, pattern:&str) -> usize {
    // 首先对模式串构建next数组
    let next_arr = kmp_next(pattern);
    let mut j:i64 = -1;
    // 我也不想把字符串先转换出来，但是必须要这么做
    let pattern:Vec<char> = pattern.chars().collect();
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

/// build kmp's next array
/// # Example
/// ```
/// let next = kmp_next("ababab");
/// ```
pub fn kmp_next(pattern:&str) -> Vec<usize> {
    let mut j:i64 = -1;
    let mut ret: Vec<usize> = vec![0];
    let pattern:Vec<char> = pattern.chars().collect();
    // 从1开始匹配是因为第零个不需要匹配
    for i in 1..pattern.len() {
        while j != -1 && pattern[(j  + 1)as usize] != pattern[i] {
            j = ret[j as usize] as i64;
        }
        if pattern[(j + 1) as usize ] == pattern[i] {
            j += 1;
        }
        ret.push(j as usize);
    }
    ret
}

/// Suffix Array
/// # Example
/// ```
/// let s="dkodkoe";
/// let sarray = sa(s)
/// ```
pub fn sa(s:&str) {
    
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
    }

    #[test]
    fn kmp_next_1() {
        assert_eq!(kmp_next("ABA"), vec![0, 0, 1]);
    }
}
