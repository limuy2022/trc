#pragma once

#include <TVM/TVMdef.h>
#include <base/trcdef.h>
#include <map>
#include <string>

namespace trc::TVM_space {
class func_ {
public:
    char* name;

    ~func_();

    /**
     * @param name 函数名
     * @warning 不会拷贝字符串，而是直接使用，不要在函数外释放字符串
     */
    explicit func_(char* name);

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
