use crate::{cfg, compiler};
use libcore::utils::check_path_type;
use rust_i18n::t;
use std::{path::PathBuf, process::exit};

pub fn build(mut opt: compiler::CompileOption) -> anyhow::Result<()> {
    let path_type = check_path_type(opt.inputsource.get_path());
    // 如果是单文件单独编译这一个文件即可
    if path_type.is_dir {
        // 否则新建build文件夹
        if std::fs::create_dir_all(cfg::BUILD_DIR_NAME).is_err() {
            eprintln!("{}", t!("common.cannot_create_build_dir"));
            exit(1);
        }
        opt.file_save = PathBuf::from(cfg::BUILD_DIR_NAME);
        // 将路径指向直接编译的文件
        opt.inputsource.set_path(PathBuf::from(cfg::MAIN_FILE));
        let mut compiler = compiler::Compiler::new(opt);
        compiler.lex()?;
    } else {
        let mut compiler = compiler::Compiler::new(opt);
        compiler.lex()?;
    }
    Ok(())
}
