/*
* 工具：输出token，调试工具
*/

#include <iostream>
#include "Compiler/Compiler.h"
#include "base/utils/filesys.h"

using namespace std;

namespace trc {
    namespace tools_in {
        void out(const string& file_name, compiler::token_lex& token_c) {
            /*
            * 格式化输出
            */
            cout << "From file " << file_name << ":\n";
            int line_index = 0;
            for (;;) {
                // 行
                const vecs& line = token_c.get_line();
                if (line.empty()) {
                    return;
                }
                cout << line_index << ":";
                line_index++;
                for (const auto& j : line) {
                    cout << j << ", ";
                }
                cout << "\n";
            }
        }

        void __out_token(const string &path) {
            string file_data;
            utils::readcode(file_data, path);
            // 解析
            compiler::token_lex token_c(file_data);
            out(path, token_c);
        }
    }

    namespace tools_out {
        void out_token(int argc, char *argv[]) {
            for (int i = 2; i < argc; ++i)
                tools_in::__out_token(argv[i]);
        }
    }
}
