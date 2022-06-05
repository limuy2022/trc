#pragma once

#include <TVM/TVMdef.h>
#include <TVMbase/library.h>
#include <TVMbase/types/base.h>
#include <base/trcdef.h>

namespace trc::TVM_space::types {
class TRC_TVMbase_api trc_float : public trcobj {
public:
    double value;

    void putline(FILE* out);

    ~trc_float();

    trc_float();

    trc_float(const double& value);

    def::OBJ to_int();

    def::OBJ to_string();

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
