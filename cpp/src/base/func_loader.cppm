/**
 * @brief 在TVM中，内置函数是通过一个编号来表示的，最大化减小体积
 */
module;
#include <array>
#include <map>
#include <string>
export module func_loader;

namespace trc::loader {
export std::array num_func = {
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
export std::map<std::string, short> func_num = {
    { "exit", 0 },
    { "print", 1 },
    { "println", 2 },
    { "input", 3 },
    { "len", 4 },
    { "help", 5 },
    { "string", 6 },
    { "int", 7 },
    { "bool", 8 },
    { "float", 9 },
    { "type", 10 },
};
}