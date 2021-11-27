/**
 *trc用于debug的工具，可以对标gcc的gdb
 */

#include <iostream>
#include <string>
#include <stack>
#include <algorithm>
#include "TVM/TVM.h"
#include "Compiler/Compiler.h"
#include "base/utils/filesys.h"
#include "base/utils/data.hpp"
#include "TVMbase/types/long.h"
#include "language/language.h"
#include "TVMbase/memory.h"
#include "base/memory/memory.h"

using namespace std;

static trc::TVM_space::TVM *vm;

static void var_lex(const string &instruction)
{
    const string &var_ = instruction.substr(instruction.find("var") + 4, instruction.length() - 3);
    if (!trc::utils::map_check_in_first(vm->dyna_data.var_names, var_))
    {
        cout << language::tdb::var << var_ << language::tdb::not_defined;
        return;
    }
    vm->dyna_data.var_names[var_]->putline(cout);
    cout << "\n";
}

static void stack_out()
{
    // 输出栈
    // 要输出就要先将数据备份
    // 注：虽然这是个对性能对着较大影响的方式，但是数据量绝对不会很大，时间损耗基本可以忽略
    // 所以这段代码无需优化
    stack<trc::def::OBJ> tmp(vm->dyna_data.stack_data);
    for (int i = 0; !tmp.empty(); ++i)
    {
        cout << i << ":";
        tmp.top()->putline(cout);
        cout << '\n';
        tmp.pop();
    }
}

static vecs cutlines(const string &code)
{
    /**
     * 将代码分割成行
     */
    vecs res;
    string tmp;
    for (auto i : code)
    {
        if (i == '\n')
        {
            res.push_back(tmp);
            tmp.clear();
        }
        else
        {
            tmp += i;
        }
    }
    if (!tmp.empty())
    {
        res.push_back(tmp);
    }
    return res;
}

static void debug(const string &code)
{
    string instruction;

    trc::compiler::Compiler(vm, code);
    // 用于输出行信息
    const vecs &out_data = cutlines(code);
    size_t n = out_data.size();
    int line_index = 0;

    while (line_index < n)
    {
        cout << "tdb>";
        getline(cin, instruction);

        if (line_index == n || instruction == "exit")
            return;
        else if (instruction == "n")
        {
            cout << out_data[line_index] << "\n";
            vm->run_step();
            cout << "\n";
        }
        else if (instruction.find("var") != string::npos)
            var_lex(instruction);
        else if (instruction == "stack")
            stack_out();
        else
            cout << language::tdb::instruction << instruction << language::tdb::not_defined;
        line_index++;
    }
}

namespace trc
{
    namespace tools_out
    {
        void tdb()
        {
            // tip
            cout << language::tdb::start_tip;
            string file_path, tmp;
            vm = TVM_space::create_TVM();
            for (;;)
            {
                // 读取需要debug的文件
                cout << "file>";
                getline(cin, file_path);
                if (file_path == "exit")
                    break;
                utils::readcode(tmp, file_path);
                debug(tmp);
                TVM_space::free_var_vm(&vm->dyna_data);
            }
            delete vm;
        }
    }
}
