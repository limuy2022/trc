/**
 * 构建ctree文件（编译）
 */

#include <Compiler/Compiler.hpp>
#include <TVM/TVM.hpp>
#include <base/ctree_loader.hpp>
#include <base/utils/filesys.hpp>
#include <filesystem>
#include <generated_params.hpp>
#include <string>
#include <tools.hpp>

namespace trc::tools {
namespace tools_in {
    void _build(TVM_space::TVM* vm, const std::string& path) {
        auto option = generate_compiler_params();
        std::string scode;
        utils::readcode(scode, path);
        compiler::Compiler(compiler::main_module, option, vm->static_data)
            .compile(scode);
        loader::save_ctree(
            vm, fs::path(path).replace_extension(".ctree").string());
    }
}

namespace tools_out {
    void build() {
        auto vm = new TVM_space::TVM;
        // 解析到命令行参数停止
        for (int i = 2; i < argc; ++i) {
            tools_in::_build(vm, argv[i]);
        }
        delete vm;
    }
}
}
