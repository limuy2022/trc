/**
 * 闪出目录下的ctree文件，
 * 删除失败后不会停止其它文件的删除
 */
#include <cstdio>
#include <iostream>
#include "../include/filesys.h"
#include "../include/cfg.h"
#include "../include/filesys.h"

using namespace std;

static int files_num, error_files;

void __clean(const string &path) {
    /**
     * 清除所有中间文件
     */

    vecs files, dirs;
    listfiles(path, "\\*.ctree", files, dirs);
    for (const auto& j : files) {
        if (!remove(path_join(2, path, j).c_str())) {
            cout << "remove file " << j << "\n";
            ++files_num;
        } else {
            cout << "can \'t remove file " << j << "\n";
            ++error_files;
        }
    }
    for (auto j : dirs) {
        __clean(j);
    }
}


int clean(int argc, char *argv[]) {
    files_num = error_files = 0;
    for (int i = 2; i < argc; ++i)
        __clean(argv[i]);
    cout << "\nremove files " << files_num << "\n";
    cerr << "can \'t remove files " << error_files << "\n";
    files_num = error_files = 0;
    return 0;
}

