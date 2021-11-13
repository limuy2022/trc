/**
*trc用于debug的工具，可以对标gcc的gdb
*/

#include <iostream>
#include <string>
#include <stack>
#include "TVM/TVM.h"
#include "Compiler/Compiler.h"
#include "read.h"
#include "data.hpp"
#include "TVM/long.h"
#include "memory/mem.h"

using namespace std;

static TVM *vm;

static void var_lex(const string &instruction) {
    const string &var_ = instruction.substr(instruction.find("var") + 4, instruction.length() - 3);
    if (!map_check_in_first(vm->var_names, var_)) {
        cout << "var " << var_ << " is not defined now.\n";
        return;
    }
    vm->var_names[var_]->putline(cout);
    cout << "\n";
}

static void stack_out() {
    int i;
    // 输出栈
    // 整型栈
    // 要输出就要先将数据备份
    // 注：虽然这是个对性能对着较大影响的方式，但是数据量绝对不会很大，时间损耗基本可以忽略
    // 所以这段代码无需优化
    stack<OBJ> tmp(vm->stack_data);
    for (int i = 0; !tmp.empty(); ++i) {
        cout << i << ":";
        tmp.top()->putline(cout);
        cout << '\n';
        tmp.pop();
    }
}

static void debug(const string &code) {
    string instruction;

    Compiler(vm, code);
    // 用于输出行信息
    vecs out_data;
    pre(out_data, code);
    size_t n = out_data.size();
    int line_index = 0;

    while (line_index < n) {
        cout << "tdb>";
        getline(cin, instruction);

        if (line_index == n || instruction == "exit")
            return;
        else if (instruction == "n") {
            cout << out_data[line_index] << "\n";
            vm->run_step();
            cout << "\n";
        } else if (instruction.find("var") != string::npos)
            var_lex(instruction);
        else if (instruction == "stack")
            stack_out();
        else
            cout << "instruction " << instruction << " is not defined.\n";
        line_index++;
    }
}

namespace tools_out {
    void tdb() {
        // tip
        cout << "trc debugger is running.You can read \'Doc\\TDB.txt to find the help.\'\n\n";
        string file_path, tmp;
        vm = create_TVM();
        for (;;) {
            // 读取需要debug的文件
            cout << "file>";
            getline(cin, file_path);
            if (file_path == "exit") break;
            readcode(tmp, file_path);
            debug(tmp);
            free_var_vm(vm);
        }
        delete vm;
    }
}
