/**
 * 构建ctree文件（编译）
 */

#include <string>
#include "TVM/TVM.h"
#include "ctree_loader.h"
#include "utils/read.h"
#include "Compiler/Compiler.h"
#include "utils/filesys.h"
#include "share.h"

using namespace std;

namespace tools_in {
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
}

namespace tools_out {
    void build(int argc, char *argv[]) {
        TVM *vm = create_TVM();
        for (int i = 2; i < argc; ++i)
            tools_in::__build(vm, argv[i]);
        delete vm;
    }
}

