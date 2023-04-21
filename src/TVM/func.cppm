module;
#include <cstddef>
export module func;
import TVMdef;
import trcdef;

namespace trc::TVM_space {
export class func_ {
public:
    char* name;

    ~func_();

    // 字节码信息
    struct_codes bytecodes;
    // 符号表长度
    size_t symbol_form_size;
};

func_::~func_() {
    delete[] name;
}
}
