#pragma once

#include <TVM/TVM_data.h>
#include <TVM/TVMdef.h>
#include <base/trcdef.h>
#include <map>
#include <string>

namespace trc::TVM_space {
class frame_ {
public:
    TVM_dyna_data dyna_data;

    frame_(func_* func);
};

class func_ {
public:
    char* name;

    ~func_();

    // 字节码信息
    struct_codes bytecodes;
    // 符号表长度
    size_t symbol_form_size;

    /**
     * @brief 执行函数
     * @param frame 函数帧
     * @return def::OBJ 返回值
     */
    def::OBJ run(frame_& frame);
};
}
