/**
*File    :   Trc.cpp
*Time    :   2021/07/09 20:49:12
*Author  :   李沐阳
*Version :   0.2
*/

#include <iostream>
#include <cstring>
#include "include/tools.h"
#include "include/memory/mem.h"

using namespace std;

namespace tools_out {
    void help() {
        /**
         * Trc命令行操作帮助文档，因为代码简单所以放在主文件
         */
        
        cout << "Trc is a programming language based on stack this project from the\n\
    most basic operators such as arithmetic, the branch structure, circulation structure, \n\
    and then realize the constant folding, has realized the type of high precision operation, \n\
    input and output as well as some commonly used built-in function, then realize the recycling, \n\
    escaping strings, local and global variables,  This paper constructs a perfect error reporting \n\
    system from compiler to interpreter, introduces the concept of module, makes the program can\n\
    be modular programming, and establishes many practical tools, such as compile, run, decompile, \n\
    batch compile, clean up files, code debugging, command line scripts and other functions\n";
    }
}

#define CMP(str, str2) (!strcmp((str), (str2)))

static void run_no_argv_pattern(const char * pattern) {
    if (CMP(pattern, "tdb")) {
        tools_out::tdb(); return;
    } if (CMP(pattern, "help")) {
        tools_out::help(); return;
    }
    // 参数不对，未输入文件或者未输入模式
    cerr << "Trc:no input files.\n";
}

static void run_yes_argv_pattern(const char * pattern, int argc, char **argv) {
    if (CMP(pattern, "crun")) {
        tools_out::crun(argc, argv); return;
    } if (pattern, "token") {
        tools_out::out_token(argc, argv); return;
    } if (CMP(pattern, "dis")) {
        tools_out::dis(argc, argv); return;
    } if (CMP(pattern, "grammar")) {
        tools_out::out_grammar(argc, argv); return;
    } if (CMP(pattern, "brun")) {
        tools_out::brun(argc, argv); return;
    } if (CMP(pattern, "build")) {
        tools_out::build(argc, argv); return;
    } if (CMP(pattern, "run")) {
        tools_out::run(argc, argv); return;
    } if (CMP(pattern, "all")) {
        tools_out::all(argc, argv); return;
    } if (CMP(pattern, "clean")) {
        tools_out::clean(argc, argv); return;
    }
    cerr << "Trc:mode \"" << pattern << "\" is not correct.\n";
}

#undef CMP

int main(int argc, char *argv[]) {
    init_mem();
    // 停止兼容stdio，提升I/O的速度
    ios_base::sync_with_stdio(false);
    switch (argc) {
        case 1:
            // 不指定模式，没有参数，默认为交互模式
            tools_out::tshell();
            break;
        case 2:
            // 指定模式，没有参数
            run_no_argv_pattern(argv[1]);
            break;
        default:
            // 检查参数，匹配调用模式
            run_yes_argv_pattern(argv[1], argc, argv);
    }
    // 执行正常情况下卸载内存，不正常的话就崩溃了
    quit_mem();
    return 0;
}
