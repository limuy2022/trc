/**
 * @file tdb.cpp
 * @brief trc用于debug的工具，可以对标gcc的gdb
 * @date 2022-04-07
 * @copyright Copyright (c) 2022
 */

#include "TVM/memory.h"
#include <Compiler/Compiler.h>
#include <TVM/TVM.h>
#include <algorithm>
#include <base/io.hpp>
#include <base/utils/data.hpp>
#include <base/utils/filesys.h>
#include <cstdio>
#include <cstring>
#include <language/language.h>
#include <stack>
#include <string>

static trc::TVM_space::TVM* vm;

/**
 * @brief 输出变量值
 * @param instruction 指令
 */
static void var_lex(const std::string& instruction) {
    const std::string& var_ = instruction.substr(
        instruction.find("var") + 4, instruction.length() - 3);
    if (!trc::utils::map_check_in_first(vm->dyna_data.var_names, var_)) {
        printf("%s%s%s", language::tdb::var, var_.c_str(),
            language::tdb::not_defined);
        return;
    }
    vm->dyna_data.var_names[var_]->putline(stdout);
    putchar('\n');
}

/**
 * @brief 设置断点
 * @param line 行号
 */
static void setbreakpoint(size_t line) {
}

/**
 * @brief 输出栈
 */
static void stack_out() {
    // 输出栈
    // 要输出就要先将数据备份
    // 注：虽然这是个对性能对着较大影响的方式，但是数据量绝对不会很大，时间损耗基本可以忽略
    // 所以这段代码无需优化
    std::stack<trc::def::OBJ> tmp(vm->dyna_data.stack_data);
    for (int i = 0; !tmp.empty(); ++i) {
        printf("%d:", i);
        tmp.top()->putline(stdout);
        putchar('\n');
        tmp.pop();
    }
}

/**
 * @brief 将代码分割成行
 * @return vecs 按行储存的代码
 */
static vecs cutlines(const std::string& code) {
    vecs res;
    std::string tmp;
    for (auto i : code) {
        if (i == '\n') {
            res.push_back(tmp);
            tmp.clear();
        } else
            tmp += i;
    }
    if (!tmp.empty()) {
        res.push_back(tmp);
    }
    return res;
}

namespace trc {
/**
 * @brief 启动调试
 * @param code 代码
 */
static void debug(const std::string& code) {
    char* instruction = nullptr;

    trc::compiler::Compiler(vm, code);
    // 用于输出代码行信息
    const vecs& out_data = cutlines(code);
    size_t n = out_data.size();
    // 指向当前执行的代码
    size_t line_index = 0;
    // 指令长度
    size_t instruction_length;

    while (line_index < n) {
        printf("%s", "tdb>");
        free(instruction);
        io::readstr(instruction, stdin);
        instruction_length = strlen(instruction);

        if (*instruction == '\0') {
            // 指令为空重新读取
            continue;
        }
        if (line_index == n || !strcmp(instruction, "exit")
            || instruction[0] == 'e') {
            /*意思是如果执行到了末尾或者遇到了退出指令*/
            return;
        } else if (instruction[0] == 'n' || !strcmp(instruction, "next")) {
            printf("%s\n", out_data[line_index].c_str());
            vm->run_line_bycode();
            putchar('\n');
            line_index++;
        } else if ((instruction_length >= 3 && !strcmp(instruction, "var"))
            || instruction[0] == 'v') {
            var_lex(instruction);
        } else if (!strcmp(instruction, "stack") || instruction[0] == 's') {
            stack_out();
        } else {
            printf("%s%s%s", language::tdb::instruction, instruction,
                language::tdb::not_defined);
        }
    }
    free(instruction);
}

namespace tools {
    namespace tools_out {
        void tdb() {
            // 输出开始提示
            puts(language::tdb::start_tip);
            char* file_path = nullptr;
            std::string tmp;
            vm = TVM_space::create_TVM();
            for (;;) {
                // 读取需要debug的文件
                printf("%s", "file>");
                free(file_path);
                io::readstr(file_path, stdin);
                if (!strcmp(file_path, "exit"))
                    break;
                utils::readcode(tmp, file_path);
                debug(tmp);
                TVM_space::free_var_vm(&vm->dyna_data);
            }
            delete vm;
            free(file_path);
        }
    }
}
}
