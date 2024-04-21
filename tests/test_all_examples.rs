//! this test all examples files and check them output

use std::fs::read_to_string;

use assert_cmd::Command;

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
            println!("{}", ans_path.display());
            let expected_res = read_to_string(ans_path).unwrap();
            let assert = cmd.arg("run").arg(path);
            assert.assert().success().stdout(expected_res);
        }
    }
}

#[test]
pub fn test_compile_examples() {
    let mut cmd = Command::cargo_bin("trc").unwrap();
}

#[test]
pub fn test_wrong_code() {
    let mut cmd = Command::cargo_bin("trc").unwrap();
}
