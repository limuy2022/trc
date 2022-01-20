/**
 * 执行编译好的ctree文件
 */

#include "Compiler/Compiler.h"
#include "TVM/TVM.h"
#include "base/ctree_loader.h"
#include "base/trcdef.h"
#include "base/utils/filesys.h"
#include "tools.h"

namespace trc::tools {
namespace tools_in {
    void __run(
        TVM_space::TVM* vm, const std::string& path) {
        if (loader::is_magic(path)) {
            /*是ctree文件*/
            loader::loader_ctree(vm, path);
        } else {
            /*是源文件*/
            std::string scodes;
            utils::readcode(scodes, path);
            compiler::Compiler(vm, scodes);
        }
        vm->run_all();
    }
}

namespace tools_out {
    void run() {
        TVM_space::TVM* vm = TVM_space::create_TVM();
        for (int i = 2; i < argc; ++i)
            tools_in::__run(vm, argv[i]);
        delete vm;
    }
}
}
