//! this test all examples files and check them output

use assert_cmd::Command;
use core::panic;
use libcore::utils::get_next_check_char;
use std::fs::read_to_string;

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
fn check_examples_eq(expected: &str, actual: &str) -> Result<(), String> {
    let mut iter1 = expected.chars();
    let mut iter2 = actual.chars();
    let checker = |c| c == '\r';
    let checker_end = |c| c == ' ' || c == '\n';
    loop {
        let a = get_next_check_char(&mut iter1, checker);
        let b = get_next_check_char(&mut iter2, checker);
        if a != b {
            return Err(format!("expected: {}, actual: {}", expected, actual));
        }
        if a.is_none() && b.is_none() {
            break;
        }
        if a.is_none() && !check_whether_end(&mut iter2, checker_end) {
            return Err(format!("expected: {}, actual: {}", expected, actual));
        }
        if b.is_none() && !check_whether_end(&mut iter1, checker_end) {
            return Err(format!("expected: {}, actual: {}", expected, actual));
        }
    }
    Ok(())
}

#[test]
pub fn test_run_examples() {
    // 遍历整个examples目录
    let mut panic_flag = false;
    let skip_list: Vec<String> = match read_to_string("examples/skip_test") {
        Err(_) => vec![],
        Ok(s) => {
            let mut ret = vec![];
            for i in s.split("\n") {
                let tmp = i.trim();
                if tmp.is_empty() {
                    continue;
                }
                ret.push(tmp.to_owned());
            }
            ret
        }
    };
    // panic!("x{}x", skip_list[0]);
    for entry in std::fs::read_dir("examples").unwrap() {
        let path = entry.unwrap().path();
        if path.is_file()
            && match path.extension() {
                Some(ext) => ext == "trc",
                None => false,
            }
        {
            let mut cmd = Command::cargo_bin("trc").unwrap();
            // 获取标准答案
            // 判断skip test
            let tmp = path.file_name().unwrap().to_str().unwrap();
            let mut skip_flag = false;
            for i in &skip_list {
                if tmp.contains(i) {
                    skip_flag = true;
                    break;
                }
            }
            if skip_flag {
                continue;
            }
            let mut ans_path = path.clone();
            ans_path.pop();
            ans_path.push("expected_result");
            ans_path.push(path.file_name().expect("not file name"));
            ans_path.set_extension("txt");
            println!("checking {}", ans_path.display());
            let expected_res = read_to_string(&ans_path).unwrap();
            let assert = cmd.arg("run").arg(path);
            let tmp = assert.assert();
            let output = tmp.get_output();
            let output = String::from_utf8_lossy(&output.stdout);
            if let Err(e) = check_examples_eq(&expected_res, &output) {
                println!("error in {}:\n{}", ans_path.display(), e);
                panic_flag = true;
            }
        }
        if panic_flag {
            panic!("failed!");
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
