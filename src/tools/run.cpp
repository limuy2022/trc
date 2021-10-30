/**
 * 执行编译好的ctree文件
 */

#include "TVM/TVM.h"
#include "ctree_loader.h"

using namespace std;

namespace tools_in {
    void __run(TVM *vm, const string &path) {
        loader_ctree(vm, path);
        vm->run();
    }
}

namespace tools_out {
    void run(int argc, char *argv[]) {
        TVM *vm = create_TVM();
        for (int i = 2; i < argc; ++i)
            tools_in::__run(vm, argv[i]);
        delete vm;
    }
}
