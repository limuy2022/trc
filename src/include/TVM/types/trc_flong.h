#pragma once

#include <TVM/TVMdef.h>
#include <TVM/library.h>
#include <TVM/types/base.h>
#include <string>

namespace trc::TVM_space::types {
class TRC_TVM_api trc_flong : public trcobj {
public:
    trc_flong(const std::string&);

    trc_flong(double init_data);

    trc_flong();

    ~trc_flong();

    void putline(FILE* out);

    RUN_TYPE_TICK gettype();

    def::INTOBJ operator==(def::OBJ value_i);

    def::INTOBJ operator!=(def::OBJ value_i);

private:
    void set_realloc(size_t num);

    // trc_flong在底层是通过char型的动态数组实现的,动态改变大小，
    // 数组大小可能不会刚好对应数位，因为出于效率的考虑，内存会按最多分配
    // 小数点以-1标识
    char* value;
    size_t n;
    const static RUN_TYPE_TICK type;
};
}
