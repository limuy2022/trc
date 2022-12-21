/**
 * 构建ctree文件（编译）
 */

#include <Compiler/Compiler.h>
#include <TVM/TVM.h>
#include <base/ctree_loader.h>
#include <base/utils/filesys.h>
#include <filesystem>
#include <generated_params.h>
#include <string>
#include <tools.h>

namespace trc::tools {
namespace tools_in {
    void __build(TVM_space::TVM* vm, const std::string& path) {
        auto option = generate_compiler_params();
        std::string scode;
        utils::readcode(scode, path);
        compiler::Compiler(vm->static_data, scode, option);
        loader::save_ctree(
            vm, fs::path(path).replace_extension(".ctree").string());
    }
}

namespace tools_out {
    void build() {
        auto vm = new TVM_space::TVM;
        // 解析到命令行参数停止
        for (int i = 2; i < argc; ++i) {
            tools_in::__build(vm, argv[i]);
        }
        delete vm;
    }
}
}
