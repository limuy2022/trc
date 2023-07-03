/**
 * @file tdb.cpp
 * @brief trc用于debug的工具，可以对标gcc的gdb
 * @date 2022-04-07
 * @copyright Copyright (c) 2022
 */

module;
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <language/language.hpp>
#include <string>
#include <vector>
export module tdb;
import Compiler;
import TVM;
import memory;
import io;
import filesys;
import generated_params;
import trcdef;
import TVM.memory;

namespace trc {
namespace tdb {
    static TVM_space::TVM* vm;

    /**
     * @brief 输出变量值
     * @param instruction 指令
     */
    static void var_lex(const std::string& instruction) {
        // const std::string& var_ = instruction.substr(
        //     instruction.find("var") + 4, instruction.length() - 3);
        // if (!trc::utils::map_check_in_first(vm->dyna_data.var_names, var_)) {
        //     printf("%s%s%s", language::tdb::var, var_.c_str(),
        //         language::tdb::not_defined);
        //     return;
        // }
        // vm->dyna_data.var_names[var_]->putline(stdout);
        // putchar('\n');
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
        size_t index = 0;
        for (def::OBJ* i = vm->dyna_data.stack_data;
             i != vm->dyna_data.stack_top_ptr + 1; ++i) {
            printf("%zu:", index);
            (*i)->putline(stdout);
            putchar('\n');
        }
    }

    /**
     * @brief 将代码分割成行
     * @return vecs 按行储存的代码
     */
    static std::vector<std::string> cutlines(const std::string& code) {
        std::vector<std::string> res;
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

    /**
     * @brief 启动调试
     * @param code 代码
     */
    static void debug(const std::string& code) {
        char* instruction = nullptr;
        TVM_space::free_TVM(vm);
        compiler::Compiler(compiler::main_module, tools::compilerOption, vm->static_data)
            .compile(code);
        vm->reload_data();
        // 用于输出代码行信息
        const std::vector<std::string>& out_data = cutlines(code);
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
}

namespace tools::tools_out {
    export void tdb() {
        // 输出开始提示
        puts(language::tdb::start_tip);
        char* file_path = nullptr;
        std::string tmp;
        tdb::vm = new TVM_space::TVM;
        for (;;) {
            // 读取需要debug的文件
            printf("%s", "file>");
            free(file_path);
            io::readstr(file_path, stdin);
            if (!strcmp(file_path, "exit"))
                break;
            utils::readcode(tmp, file_path);
            tdb::debug(tmp);
        }
        delete tdb::vm;
        free(file_path);
    }
}
}
