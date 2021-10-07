/**
 * 编译且保存
 * 注：先编译再保存
 */

#include <string>
#include "../include/read.h"
#include "../include/TVM/TVM.h"
#include "../include/Compiler/Compiler.h"
#include "../include/ctree_loader.h"
#include "../include/filesys.h"
#include "../include/share.h"

using namespace std;

void __brun(TVM *vm, const string &path) {
    string scodes, temp;
    readcode(scodes, path);
    
    run_env::set_module(path);
    Compiler(vm, scodes);
    vm->run();
    save_ctree(vm, path_last(path, ".ctree"));
}

int brun(int argc, char *argv[]) {
    /*
    *将文件执行后把编译结果写入文件
    */
    TVM *vm = create_TVM();
    for (int i = 2; i < argc; ++i)
        __brun(vm, argv[i]);
    delete vm;
    return 0;
}
