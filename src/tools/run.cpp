/**
 * 执行编译好的ctree文件
 */

#include "../include/read.h"
#include "../include/TVM/TVM.h"
#include "../include/ctree_loader.h"

using namespace std;

void __run(TVM *vm, const string &path) {
    loader_ctree(vm, path);
    vm->run();
}

int run(int argc, char *argv[]) {
    TVM *vm = create_TVM();
    for (int i = 2; i < argc; ++i)
        __run(vm, argv[i]);
    delete vm;
    return 0;
}
