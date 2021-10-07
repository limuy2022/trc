/**
 * 执行，但不生成中间文件
 */

#include "../include/TVM/TVM.h"
#include "../include/Compiler/Compiler.h"
#include "../include/read.h"
#include "../include/share.h"

using namespace std;

void __crun(TVM *vm_run, const string &path) {
    /**
     * 执行源代码
     * path：文件路径
     */

    string scodes;
    readcode(scodes, path);

    run_env::set_module(path);
    Compiler(vm_run, scodes);
    vm_run->run();
}

int crun(int argc, char *argv[]) {
    /*
    *将文件编译后执行
    */

    TVM *vm_run = create_TVM();
    for (int i = 2; i < argc; ++i)
        __crun(vm_run, argv[i]);
    delete vm_run;
    return 0;
}
