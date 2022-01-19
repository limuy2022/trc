/**
*File    :   Trc.cpp
*Time    :   2021/07/09 20:49:12
*Author  :   李沐阳
*Version :   0.2
*/

#include <iostream>
#include <cstring>
#include <string>
#include "tools.h"
#include "base/utils/data.hpp"
#include "language/language.h"
#include "base/memory/memory.h"

using namespace std;

static inline void show_error(char *mode) {
    /**
     * 这个函数作用是报错
     */ 
    cerr << mode << language::trc::mode_not_found;
}

typedef void (*no_argv_func)();
static pair<const char*, no_argv_func> no_argv[] = {
    {"tdb", trc::tools_out::tdb},
    {"help", trc::tools_out::help}
};

typedef void (*yes_argv_func)(int, char**);
static pair<const char*, yes_argv_func> yes_argv[] {
    {"run", trc::tools_out::run},
    {"token", trc::tools_out::out_token},
    {"dis", trc::tools_out::dis},
    {"grammar", trc::tools_out::out_grammar},
    {"brun", trc::tools_out::brun},
    {"build", trc::tools_out::build}
};

static inline void run_no_argv_mode(char *mode) {
    for(int i = 0, end = trc::utils::get_index_static_array(no_argv); i < end; ++i) {
        if(!strcmp(mode, no_argv[i].first)) {
            // 匹配上了
            no_argv[i].second();
            return;
        }
    }
    show_error(mode);
}

static inline void run_yes_argv_mode(char *mode, int argc, char **argv) {
    for(int i = 0, end = trc::utils::get_index_static_array(yes_argv); i < end; ++i) {
        if(!strcmp(mode, yes_argv[i].first)) {
            // 匹配上了
            yes_argv[i].second(argc, argv);
            return;
        }
    }
    show_error(mode);
}

int main(int argc, char *argv[]) {
    trc::memory::init_mem();
    // 停止兼容stdio，提升I/O的速度
    ios_base::sync_with_stdio(false);
    switch (argc) {
        case 1:
            // 不指定模式，没有参数，默认为交互模式
            trc::tools_out::tshell();
            break;
        case 2:
            // 指定模式，没有参数
            run_no_argv_mode(argv[1]);
            break;
        default:
            // 检查参数，匹配调用模式
            run_yes_argv_mode(argv[1], argc, argv);
    }
    // 执行正常情况下卸载内存，不正常的话就崩溃了
    trc::memory::quit_mem();
    return 0;
}
