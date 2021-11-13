/**
 * 在TVM中，内置函数是通过一个编号来表示的，最大化减小体积
 */
/*********************************************************
 * 注意：如果修改此处文件，请一并修改头文件中的内置函数数量宏 *
 *********************************************************/

#include <map>
#include <string>

using namespace std;

// 编码转函数
string num_func[] = {
        "exit",
        "print",
        "println",
        "input",
        "len",
        "help",
        "string",
        "int",
        "bool",
        "float",
        "type",
};

// 函数转编码
map<string, short> func_num = {
        {"exit",    0},
        {"print",   1},
        {"println", 2},
        {"input",   3},
        {"len",     4},
        {"help",    5},
        {"string",  6},
        {"int",     7},
        {"bool",    8},
        {"float",   9},
        {"type",    10},
};

