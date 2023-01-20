#pragma once

#include <TVM/TVMdef.hpp>
#include <TVM/types/base.hpp>
#include <string>

inline constexpr size_t FLOAT_LONGFLOAT_LINE = 15;

namespace trc::TVM_space::types {
class trc_flong : public trcobj {
public:
    explicit trc_flong(const std::string&);

    explicit trc_flong(double init_data);

    trc_flong();

    ~trc_flong() override;

    void putline(FILE* out) override;

    RUN_TYPE_TICK gettype() override;

    def::INTOBJ operator==(def::OBJ value_i) override;

    def::INTOBJ operator!=(def::OBJ value_i) override;

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
