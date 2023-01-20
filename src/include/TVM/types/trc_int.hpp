#pragma once

#include <TVM/TVMdef.hpp>
#include <TVM/types/base.hpp>
#include <base/trcdef.hpp>
#include <cstdint>

namespace trc::TVM_space::types {
typedef int32_t trc_int_t;
class trc_int : public trcobj {
public:
    trc_int_t value = 0;

    void putline(FILE* out) override;

    ~trc_int() override;

    trc_int();

    trc_int(int value);

    def::OBJ to_string() override;

    def::OBJ to_float() override;

    def::OBJ to_bool() override;

    def::INTOBJ operator==(def::OBJ value_i) override;

    def::INTOBJ operator!=(def::OBJ value_i) override;

    def::INTOBJ operator<(def::OBJ value_i) override;

    def::INTOBJ operator>(def::OBJ value_i) override;

    def::INTOBJ operator<=(def::OBJ value_i) override;

    def::INTOBJ operator>=(def::OBJ value_i) override;

    def::OBJ operator+(def::OBJ value_i) override;

    def::OBJ operator-(def::OBJ value_i) override;

    def::OBJ operator*(def::OBJ value_i) override;

    def::OBJ operator/(def::OBJ value_i) override;

    def::OBJ operator%(def::OBJ value_i) override;

    def::OBJ pow_(def::OBJ value_i) override;

    def::OBJ zdiv(def::OBJ value_i) override;

    def::INTOBJ operator!() override;

    def::INTOBJ operator&&(def::OBJ value_i) override;

    def::INTOBJ operator||(def::OBJ value_i) override;

    RUN_TYPE_TICK gettype() override;

private:
    // 整型标记类型
    const static RUN_TYPE_TICK type;
};
}
