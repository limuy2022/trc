//! this test all examples files and check them output

use std::fs::read_to_string;

use assert_cmd::Command;
use trc::base::utils::get_next_check_char;

/// 检查迭代器是否剩下的所有字符都满足某个条件
fn check_whether_end(iter: &mut impl Iterator<Item = char>, condit: impl Fn(char) -> bool) -> bool {
    loop {
        match iter.next() {
            Some(c) => {
                if !condit(c) {
                    return false;
                }
            }
            None => return true,
        }
    }
}

/// 判断输出是否相同，忽略行末空格和最后的空行，自动处理\r\n等情况
fn check_examples_eq(expected: &str, actual: &str) {
    let mut iter1 = expected.chars();
    let mut iter2 = actual.chars();
    let checker = |c| c == '\r';
    let checker_end = |c| c == ' ' || c == '\n';
    loop {
        let a = get_next_check_char(&mut iter1, checker);
        let b = get_next_check_char(&mut iter2, checker);
        if a != b {
            panic!("expected: {}, actual: {}", expected, actual);
        }
        if a.is_none() && b.is_none() {
            break;
        }
        if a.is_none() && !check_whether_end(&mut iter2, checker_end) {
            panic!("expected: {}, actual: {}", expected, actual);
        }
        if b.is_none() && !check_whether_end(&mut iter1, checker_end) {
            panic!("expected: {}, actual: {}", expected, actual);
        }
    }
}

#[test]
pub fn test_run_examples() {
    // 遍历整个examples目录
    for entry in std::fs::read_dir("examples").unwrap() {
        let path = entry.unwrap().path();
        if path.is_file() {
            let mut cmd = Command::cargo_bin("trc").unwrap();
            // 获取标准答案
            let mut ans_path = path.clone();
            ans_path.pop();
            ans_path.push("expected_result");
            ans_path.push(path.file_name().expect("not file name"));
            ans_path.set_extension("txt");
            println!("checking {}", ans_path.display());
            let expected_res = read_to_string(ans_path).unwrap();
            let assert = cmd.arg("run").arg(path);
            let tmp = assert.assert();
            let output = tmp.get_output();
            let output = String::from_utf8_lossy(&output.stdout);
            check_examples_eq(&expected_res, &output);
        }
    }
}

#[test]
pub fn test_compile_examples() {
    let _cmd = Command::cargo_bin("trc").unwrap();
}

#[test]
pub fn test_wrong_code() {
    let _cmd = Command::cargo_bin("trc").unwrap();
}
