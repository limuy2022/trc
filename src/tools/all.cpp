/**
 * 编译目录下所有tree文件,已经编译过的，不论修改时间，重新编译
 * 注：该工具编译中一旦出现一个文件错误，工具立即停止编译
 */

#include <iostream>
#include <string>
#include "TVM/TVM.h"
#include "tools.h"
#include "cfg.h"
#include "filesys.h"

using namespace std;

static int files_num;

namespace tools_out {
    void all(int argc, char *argv[]) {
        files_num = 0;
        TVM* vm = create_TVM();
        for (int i = 2; i < argc; ++i)
            tools_in::__all(vm, argv[i]);
        cout << "\nbuild " << files_num << " files\n";
        files_num = 0;
        delete vm;
    }
}

namespace tools_in {
    void __all(TVM *vm, const string &path) {
        vecs files, dirs;
        listfiles(path, "\\*.tree", files, dirs);
        for (const auto& j : files) {
            const string& p(path_join(2, path, j));
            __build(vm, p);
            cout << "build file " << p << "\n";
            ++files_num;
        }
        for (const auto& j : dirs)
            __all(vm, j);
    }
}


