/**
 * 构建ctree文件（编译）
 */

module;
#include <filesystem>
#include <string>
export module build;
import TVM;
import ctree_loader;
import filesys;
import generated_params;
import cmdparser;
import basic_def;
import compiler_def;
import data;
import unreach;
import color;
import help;
import compile_env;
import compiler;

export namespace trc::tools {
namespace tools_in {
    /**
     * @brief 负责编译工作的底层函数
     * @param vm 填充的虚拟机
     * @param path 文件的路径
     */
    void _build(TVM_space::TVM* vm, const std::string& path) {
        compiler::compiler().parse(
            tools::compilerOption, path, &vm->static_data);
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
