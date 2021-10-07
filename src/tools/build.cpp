/**
 * 构建ctree文件（编译）
 */

#include <string>
#include "../include/TVM/TVM.h"
#include "../include/ctree_loader.h"
#include "../include/read.h"
#include "../include/Compiler/Compiler.h"
#include "../include/filesys.h"
#include "../include/share.h"

using namespace std;

void __build(TVM *vm, const string &path) {
    /**
     * 负责编译工作的底层函数
     * vm：填充的虚拟机
     * path：文件的路径
     */

    string scode;
    readcode(scode, path);
    // 设置当前编译模块路径，便于报错
    run_env::set_module(path);
    Compiler(vm, scode);
    save_ctree(vm, path_last(path, ".ctree"));
}

int build(int argc, char *argv[]) {
    TVM *vm = create_TVM();
    for (int i = 2; i < argc; ++i)
        __build(vm, argv[i]);
    delete vm;
    return 0;
}
