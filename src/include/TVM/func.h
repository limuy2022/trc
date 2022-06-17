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

    frame_();
};

class func_ {
public:
    char* name;

    ~func_();

    // 字节码信息
    struct_codes bytecodes;

    /**
     * @brief 执行函数
     * @param frame 函数帧
     * @return def::OBJ 返回值
     */
    def::OBJ run(frame_& frame);
};
}
