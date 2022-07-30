#pragma once

#include <TVM/TVMdef.h>
#include <TVM/library.h>
#include <TVM/types/base.h>
#include <base/trcdef.h>
#include <cstdint>

namespace trc::TVM_space::types {
typedef int32_t trc_int_t;
class TRC_TVM_api trc_int : public trcobj {
public:
    trc_int_t value = 0;

    void putline(FILE* out);

    ~trc_int();

    trc_int();

    trc_int(int value);

    def::OBJ to_string();

    def::OBJ to_float();

    def::OBJ to_bool();

    def::INTOBJ operator==(def::OBJ value_i);

    def::INTOBJ operator!=(def::OBJ value_i);

    def::INTOBJ operator<(def::OBJ value_i);

    def::INTOBJ operator>(def::OBJ value_i);

    def::INTOBJ operator<=(def::OBJ value_i);

    def::INTOBJ operator>=(def::OBJ value_i);

    def::OBJ operator+(def::OBJ value_i);

    def::OBJ operator-(def::OBJ value_i);

    def::OBJ operator*(def::OBJ value_i);

    def::OBJ operator/(def::OBJ value_i);

    def::OBJ operator%(def::OBJ value_i);

    def::OBJ pow_(def::OBJ value_i);

    def::OBJ zdiv(def::OBJ value_i);

    def::INTOBJ operator!();

    def::INTOBJ operator&&(def::OBJ value_i);

    def::INTOBJ operator||(def::OBJ value_i);

    RUN_TYPE_TICK gettype();

private:
    // 整型标记类型
    const static RUN_TYPE_TICK type;
};
}
