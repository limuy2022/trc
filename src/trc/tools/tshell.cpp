/**
 * trc交互式界面，无需文件即可执行
 */

#include <string>
#include <iostream>
#include "TVM/TVM.h"
#include "Compiler/Compiler.h"
#include "base/Error.h"

using namespace std;

static inline bool is_block(const string &code) {
    /**
     * 是否为新的语句块开始
     */
    return code.back() == '{';
}
static inline bool is_end(const string &code) {
    /**
     * 语句块是否结束
     */
    return code.back() == '}';
}

static void get_block(string &res) {
    string temp;
    int break_num = 1;
    for (;;) {
        for (int i = 0; i <= break_num; ++i)
            cout << "    ";
        cout << "->";
        getline(cin, temp);
        res += temp + "\n";
        if (is_block(temp))
            ++break_num;
        else if (is_end(temp)) {
            --break_num;
            if (!break_num)
                return;
        }
    }
}

namespace trc {
    namespace tools_out {
        void tshell() {
            /*
            * trc的交互式界面
            */
            cout << "Trc "<< def::version << "\n\n";

            string code;

            TVM_space::TVM *vm = TVM_space::create_TVM();
            // tshell报错但不终止程序
            error::error_env::quit = false;
            for (;;) {
                cout << "\ntshell>";
                getline(cin, code);
                if (code == "exit()")
                    break;
                code += "\n";
                if (is_block(code)) {
                    get_block(code);
                }
                compiler::Compiler(vm, code);
                vm->run();
            }
            delete vm;
            error::error_env::quit = true;
        }
    }
}
