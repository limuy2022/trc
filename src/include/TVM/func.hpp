#pragma once

#include <TVM/TVMdef.hpp>
#include <base/trcdef.hpp>

namespace trc::TVM_space {
class func_ {
public:
    char* name;

    ~func_();

    // 字节码信息
    struct_codes bytecodes;
    // 符号表长度
    size_t symbol_form_size;
};

class frame_ {
public:
    void set_func(const func_& func);
    void free_func();
    // 保存函数的名字
    const char* funcname;
    // 符号表
    symbol_form localvars;
};
}
