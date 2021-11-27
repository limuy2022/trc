/**
 * 执行编译好的ctree文件
 */

#include "TVM/TVM.h"
#include "base/ctree_loader.h"
#include "Compiler/Compiler.h"
#include "base/utils/filesys.h"
#include "base/trcdef.h"
#include "base/ctree_loader.h"

using namespace std;

namespace trc {
    namespace tools_in {
        void __run(TVM_space::TVM *vm, const string &path) {
            if(loader::is_magic(path)) {
                /*是ctree文件*/
                loader::loader_ctree(vm, path);
            } else {
                /*是源文件*/
                string scodes;
                utils::readcode(scodes, path);
                run_env::set_module(path);
                compiler::Compiler(vm, scodes);
            }
            vm->run();
        }
    }

    namespace tools_out {
        void run(int argc, char *argv[]) {
            TVM_space::TVM *vm = TVM_space::create_TVM();
            for (int i = 2; i < argc; ++i)
                tools_in::__run(vm, argv[i]);
            delete vm;
        }
    }
}
