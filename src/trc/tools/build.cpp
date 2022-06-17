/**
 * 构建ctree文件（编译）
 */

#include <Compiler/Compiler.h>
#include <TVM/TVM.h>
#include <base/ctree_loader.h>
#include <base/utils/filesys.h>
#include <filesystem>
#include <gflags/gflags.h>
#include <string>
#include <tools.h>

namespace fs = std::filesystem;

namespace trc::tools {
namespace tools_in {
    void __build(TVM_space::TVM* vm, const std::string& path) {
        std::string scode;
        utils::readcode(scode, path);
        compiler::Compiler(vm, scode);
        loader::save_ctree(
            vm, fs::path(path).replace_extension(".ctree").string());
    }
}

// 行号表参数
DEFINE_bool(noline_number_table, false, "Controls whether a line number");
namespace tools_out {
    void build() {
        TVM_space::TVM* vm = TVM_space::create_TVM();
        // 解析到命令行参数停止
        for (int i = 2; i < argc && argv[i][0] != '-'; ++i) {
            tools_in::__build(vm, argv[i]);
        }
        delete vm;
    }
}
}
