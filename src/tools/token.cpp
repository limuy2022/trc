/*
* 工具：输出token，调试工具
*/

#include <iostream>
#include "Compiler/Compiler.h"
#include "read.h"

using namespace std;

static void out(const string &file_name, const vecs2d &data) {
    /*
    * 格式化输出
    */
    cout << "From file " << file_name << ":" << "\n";
    int line_index = 0;
    for (const auto& line : data) {
        // 行
        cout << line_index << ":";
        line_index++;
        for (const auto& j : line) {
            cout << j << ", ";
        }
        cout << "\n";
    }
}

namespace tools_in {
    void __out_token(const string &path) {
        string file_data;
        readcode(file_data, path);
        // 解析
        vecs start_temp;
        pre(start_temp, file_data);
        const auto& temp_token = final_token(start_temp);
        out(path, temp_token);
    }
}

namespace tools_out {
    void out_token(int argc, char *argv[]) {
        for (int i = 2; i < argc; ++i)
            tools_in::__out_token(argv[i]);
    }
}
