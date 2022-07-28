/**
 * 在TVM中，内置函数是通过一个编号来表示的，最大化减小体积
 */

#include <array>
#include <base/func_loader.h>
#include <map>
#include <string>

namespace trc::loader {
// 编码转函数
std::array<std::string, 12> num_func = {
    "",// 零号位空置防止指令运行冲突
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
std::map<std::string, short> func_num = {
    { "exit", 1 },
    { "print", 2 },
    { "println", 3 },
    { "input", 4 },
    { "len", 5 },
    { "help", 6 },
    { "string", 7 },
    { "int", 8 },
    { "bool", 9 },
    { "float", 10 },
    { "type", 11 },
};
}
