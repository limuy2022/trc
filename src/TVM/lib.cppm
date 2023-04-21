/**
 * 与TVM库文件有关的定义
 */

module;
#include <array>
#include <map>
#include <string>
export module lib;
import trcdef;

export namespace trc::TVM_space::cpp_libs {
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

cpp_lib::cpp_lib(int funcnums)
    : funcs(new char*[funcnums])
    , funcs_nums(funcnums) {
}

cpp_lib::~cpp_lib() {
    for (int i = 0; i < funcs_nums; ++i) {
        delete[] funcs[i];
    }
    delete[] funcs;
}

std::map<std::string, cpp_lib> load_cpp_libs;
std::array<std::string, 1> names = { "math" };
}
