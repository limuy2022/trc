/**
 * 与TVM库文件有关的定义
 */

#pragma once

#include <base/trcdef.h>
#include <map>

namespace trc::TVM_space::cpp_libs {
/**
 * 关于外部调用库
 */
class cpp_lib {
    /**
     * c++编写的标准库的信息描述，便于进行调用
     */

public:
    // 函数名的定义
    char** funcs;

    int funcs_nums;

    std::map<std::string, def::OBJ> vars;

    cpp_lib(int);

    ~cpp_lib();
};

extern std::map<std::string, cpp_lib> load_cpp_libs;
extern std::array<std::string, 1> names;
}
