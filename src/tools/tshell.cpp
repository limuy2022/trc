/**
 * trc交互式界面，无需文件即可执行
 */

#include <string>
#include <iostream>
#include "cfg.h"
#include "TVM/TVM.h"
#include "Compiler/Compiler.h"
#include "Error.h"

// 是否为新的语句块开始
#define is_block(code) ((code).back() == '{')
// 语句块是否结束
#define is_end(code) ((code).back() == '}')

using namespace std;

static void get_block(string &res) {
    string temp;
    int break_num = 1;
    for(;;) {
        for (int i = 0; i <= break_num; ++i)
            cout << "    ";
        cout << "->";
        getline(cin, temp);
        res += temp;
        res += "\n";
        if (is_block(temp))
            ++break_num;
        else if (is_end(temp)) {
            --break_num;
            if (!break_num)
                return;
        }
    }
}

namespace tools_out {
    void tshell() {
        /*
        * trc的交互式界面
        */
        cout << "Welcome to Trc.This is tshell.you can use \"help()\" to find help.\n"\
        << "Version:" << version << "\n\n";
    
        string code;
        
        TVM *vm = create_TVM();
        // tshell报错但不终止程序
        error_env::quit = false;
        for(;;) {
            cout << "\ntshell>";
            getline(cin, code);
            if (code == "exit()")
                break;
            else if (is_block(code)) {
                code += "\n";
                get_block(code);
            } else
                code += "\n";
            
            Compiler(vm, code);
            vm->run();
        }
        delete vm;
        error_env::quit = true;
    }
}

#undef is_block
#undef is_end
