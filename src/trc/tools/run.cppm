/**
 * 执行编译好的ctree文件
 */

module;
#include <string>
#include <vector>
export module run;
import Compiler;
import TVM;
import ctree_loader;
import filesys;
import generated_params;
import cmdparser;
import basic_def;

export namespace trc::tools {
namespace tools_in {
    void _run(TVM_space::TVM* vm, const std::string& path) {
        if (loader::is_magic(path)) {
            /*是ctree文件*/
            loader::loader_ctree(vm, path);
        } else {
            /*是源文件*/
            std::string scodes;
            utils::readcode(scodes, path);
            compiler::Compiler(
                compiler::main_module, tools::compilerOption, vm->static_data)
                .compile(scodes);
        }
        vm->reload_data();
        vm->run_all();
    }
}

namespace tools_out {
    void run() {
        auto vm = new TVM_space::TVM;
        for (int i = cmdparser::optind + 1; i < argc; ++i) {
            tools_in::_run(vm, argv[i]);
        }
        delete vm;
    }
}
}
