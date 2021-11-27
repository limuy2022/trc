/**
 * 编译且保存
 * 注：先编译再保存
 */

#include <string>
#include "TVM/TVM.h"
#include "Compiler/Compiler.h"
#include "base/ctree_loader.h"
#include "base/utils/filesys.h"
#include "base/trcdef.h"

using namespace std;

namespace trc {
    namespace tools_in {
        void __brun(TVM_space::TVM *vm, const string &path) {
            string scodes;
            utils::readcode(scodes, path);

            run_env::set_module(path);
            compiler::Compiler(vm, scodes);
            vm->run();
            loader::save_ctree(vm, utils::path_last(path, ".ctree"));
        }
    }

    namespace tools_out {
        void brun(int argc, char *argv[]) {
            /*
            *将文件执行后把编译结果写入文件
            */
            TVM_space::TVM *vm = TVM_space::create_TVM();
            for (int i = 2; i < argc; ++i)
                tools_in::__brun(vm, argv[i]);
            delete vm;
        }
    }
}
