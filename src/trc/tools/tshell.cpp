﻿/**
 * trc交互式界面，无需文件即可执行
 */

#include "Compiler/Compiler.h"
#include "TVM/TVM.h"
#include "base/Error.h"
#include "base/io.hpp"
#include <cstdio>
#include <string>

/**
 * @brief 判断是否为新的语句块开始
 */
static inline bool is_block(const std::string& code) {
    return code.back() == '{';
}

/**
 * @brief 判断语句块是否结束
 */
static inline bool is_end(const std::string& code) {
    return code.back() == '}';
}

namespace trc {
/**
 * @brief
 * 由于整个语句块必须一起传递给token_lex解析，一起收集数据并且更改命令提示标志为->
 * @param res 语句块储存
 */
static void get_block(std::string& res) {
    char* temp = nullptr;
    int break_num = 1;
    for (;;) {
        for (int i = 0; i <= break_num; ++i)
            printf("%s", "    ");
        printf("->");
        free(temp);
        io::readstr(temp);
        res += temp;
        res += '\n';
        if (is_block(temp))
            ++break_num;
        else if (is_end(temp)) {
            --break_num;
            if (!break_num)
                return;
        }
    }
    free(temp);
}
namespace tools::tools_out {
    void tshell() {
        printf("Trc %.1f\n\n", def::version);

        char* code = nullptr;

        TVM_space::TVM* vm = TVM_space::create_TVM();
        // tshell报错但不终止程序
        error::error_env::quit = false;
        for (;;) {
            printf("%s", "\ntshell>");
            free(code);
            io::readstr(code);
            std::string code_str(code);
            if (code_str == "exit()")
                break;
            if (is_block(code)) {
                get_block(code_str);
            }
            compiler::Compiler(vm, code_str);
            vm->run_all();
        }
        free(code);
        delete vm;
        error::error_env::quit = true;
    }
}
}