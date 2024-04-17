use crate::cfg;
use colored::Colorize;

pub fn new_project(name: &str) -> anyhow::Result<()> {
    // 新建项目，名为name
    std::fs::create_dir_all(name)?;
    // 新建src文件夹
    std::fs::create_dir_all(format!("{}/src", name))?;
    // 新建main.trc
    std::fs::write(format!("{}/{}", name, cfg::MAIN_FILE), "")?;
    // 新建.gitignore
    std::fs::write(format!("{}/.gitignore", name), "trcbuild/")?;
    // 尝试git init
    let output = std::process::Command::new("git")
        .args(["init"])
        .current_dir(name)
        .output()
        .expect("git init failed");
    if !output.status.success() {
        println!("{}", "Git not found".green());
    }
    Ok(())
}
