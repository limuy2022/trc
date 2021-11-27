/**
*File    :   Trc.cpp
*Time    :   2021/07/09 20:49:12
*Author  :   李沐阳
*Version :   0.2
*/

#include <iostream>
#include <cstring>
#include "tools.h"
#include "language/language.h"
#include "base/memory/memory.h"

using namespace std;

static void run_no_argv_mode(char *mode, char *argv[]) {
    if (!strcmp(mode, "tdb")) {
        trc::tools_out::tdb();
    } else if (!strcmp(mode, "help")) {
        trc::tools_out::help();
    } else{
        // 参数不对，默认为run模式
        // 例如：trc yyy之类的模式将会把yyy当成文件执行
        trc::tools_out::run(2, argv);
    }
}

static void run_yes_argv_mode(char *mode, int argc, char **argv) {
    if (!strcmp(mode, "run")) {
        trc::tools_out::run(argc, argv);
    } else if (!strcmp(mode, "token")) {
        trc::tools_out::out_token(argc, argv);
    } else if (!strcmp(mode, "dis")) {
        trc::tools_out::dis(argc, argv);
    } else if (!strcmp(mode, "grammar")) {
        trc::tools_out::out_grammar(argc, argv);
    } else if (!strcmp(mode, "brun")) {
        trc::tools_out::brun(argc, argv);
    } else if (!strcmp(mode, "build")) {
        trc::tools_out::build(argc, argv);
    } else {
        cerr << mode << language::trc::mode_not_found;
    }
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
            run_no_argv_mode(argv[1], argv);
            break;
        default:
            // 检查参数，匹配调用模式
            run_yes_argv_mode(argv[1], argc, argv);
    }
    // 执行正常情况下卸载内存，不正常的话就崩溃了
    trc::memory::quit_mem();
    return 0;
}
