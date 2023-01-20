/**
 * 执行编译好的ctree文件
 */

#include <Compiler/Compiler.hpp>
#include <TVM/TVM.hpp>
#include <base/ctree_loader.hpp>
#include <base/utils/filesys.hpp>
#include <generated_params.hpp>
#include <tools.hpp>

namespace trc::tools {
namespace tools_in {
    void _run(TVM_space::TVM* vm, const std::string& path) {
        if (loader::is_magic(path)) {
            /*是ctree文件*/
            loader::loader_ctree(vm, path);
        } else {
            /*是源文件*/
            std::string scodes;
            utils::readcode(scodes, path);
            auto option = generate_compiler_params();
            compiler::Compiler(compiler::main_module, option, vm->static_data)
                .compile(scodes);
        }
        vm->reload_data();
        vm->run_all();
    }
}

namespace tools_out {
    void run() {
        auto vm = new TVM_space::TVM;
        for (int i = 2; i < argc; ++i) {
            tools_in::_run(vm, argv[i]);
        }
        delete vm;
    }
}
}
