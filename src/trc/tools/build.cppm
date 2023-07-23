/**
 * 构建ctree文件（编译）
 */

module;
#include <filesystem>
#include <string>
export module build;
import Compiler;
import TVM;
import ctree_loader;
import filesys;
import generated_params;
import cmdparser;
import basic_def;
import compiler_def;
import token;
import data;
import unreach;
import color;
import help;
import compile_env;

export namespace trc::tools {
namespace tools_in {
    /**
     * @brief 负责编译工作的底层函数
     * @param vm 填充的虚拟机
     * @param path 文件的路径
     */
    void _build(TVM_space::TVM* vm, const std::string& path) {
        std::string scode;
        utils::readcode(scode, path);
        compiler::Compiler(
            compiler::main_module, tools::compilerOption, vm->static_data)
            .compile(scode);
        loader::save_ctree(
            vm, fs::path(path).replace_extension(".ctree").string());
    }
}

namespace tools_out {
    void build() {
        auto vm = new TVM_space::TVM;
        for (int i = cmdparser::optind + 1; i < argc; ++i) {
            tools_in::_build(vm, argv[i]);
        }
        delete vm;
    }
}
}
