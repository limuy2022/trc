/**
 * 执行编译好的ctree文件
 */

#include <Compiler/Compiler.h>
#include <TVM/TVM.h>
#include <base/ctree_loader.h>
#include <base/utils/filesys.h>
#include <generated_params.h>
#include <tools.h>

namespace trc::tools {
namespace tools_in {
    void __run(TVM_space::TVM* vm, const std::string& path) {
        if (loader::is_magic(path)) {
            /*是ctree文件*/
            loader::loader_ctree(vm, path);
        } else {
            /*是源文件*/
            std::string scodes;
            utils::readcode(scodes, path);
            auto option = generate_compiler_params();
            compiler::Compiler(vm, scodes, &option);
        }
        vm->reload_data();
        vm->run_all();
    }
}

namespace tools_out {
    void run() {
        auto vm = new TVM_space::TVM;
        for (int i = 2; i < argc; ++i) {
            tools_in::__run(vm, argv[i]);
        }
        delete vm;
    }
}
}
