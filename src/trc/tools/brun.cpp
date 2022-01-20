/**
 * 编译且保存
 * 注：先编译再保存
 */

#include "Compiler/Compiler.h"
#include "TVM/TVM.h"
#include "base/ctree_loader.h"
#include "base/utils/filesys.h"
#include "tools.h"
#include <string>

namespace trc::tools {
namespace tools_in {
    void __brun(
        TVM_space::TVM* vm, const std::string& path) {
        __build(vm, path);
        vm->run_all();
    }
}

namespace tools_out {
    void brun() {
        TVM_space::TVM* vm = TVM_space::create_TVM();
        for (int i = 2; i < argc; ++i)
            tools_in::__brun(vm, argv[i]);
        delete vm;
    }
}
}
