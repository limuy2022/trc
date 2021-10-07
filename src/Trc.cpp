/**
*File    :   Trc.cpp
*Time    :   2021/07/09 20:49:12
*Author  :   李沐阳
*Version :   0.2
*/

#include <iostream>
#include "include/tools.h"
#include "include/memory/mem.h"

using namespace std;

int help() {
    /**
     * Trc命令行操作帮助文档
     */
    
    cout << "Trc is a programming language based on stack this project from the\n\
most basic operators such as arithmetic, the branch structure, circulation structure, \n\
and then realize the constant folding, has realized the type of high precision operation, \n\
input and output as well as some commonly used built-in function, then realize the recycling, \n\
escaping strings, local and global variables,  This paper constructs a perfect error reporting \n\
system from compiler to interpreter, introduces the concept of module, makes the program can\n\
be modular programming, and establishes many practical tools, such as compile, run, decompile, \n\
batch compile, clean up files, code debugging, command line scripts and other functions\n";
    return 0;
}


int main(int argc, char *argv[]) {
    init_mem();
    // 停止兼容stdio，提升I/O的速度
    ios_base::sync_with_stdio(false);

    string pattern;
    switch (argc) {
        case 1:
            // 不指定模式，没有参数，默认为交互模式
            return tshell();
        case 2:
            pattern = argv[1];
            if (pattern == "tdb") {
                return tdb();
            } else if (pattern == "help") {
                return help();
            } else {
                // 参数不对，未输入文件或者未输入模式
                cerr << "Trc:no input files.\n";
                return 1;
            }
        default:
            pattern = argv[1];
            // 检查参数，匹配调用模式
            if (pattern == "crun") {
                return crun(argc, argv);
            } else if (pattern == "token") {
                return out_token(argc, argv);
            } else if (pattern == "dis") {
                return dis(argc, argv);
            } else if (pattern == "grammar") {
                return out_grammar(argc, argv);
            } else if (pattern == "brun") {
                return brun(argc, argv);
            } else if (pattern == "build") {
                return build(argc, argv);
            } else if (pattern == "run") {
                return run(argc, argv);
            } else if (pattern == "all") {
                return all(argc, argv);
            } else if (pattern == "clean") {
                return clean(argc, argv);
            } else {
                cerr << "Trc:pattern \"" << pattern << "\" is not correct.\n";
                return 1;
            }
    }
    quit_mem();
    return 0;
}
