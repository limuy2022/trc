module;
#include <cmath>
#include <cstdio>
export module trc_float;
import TVMdef;
import base;
import trcdef;
import TRE;
import trc_int;

export namespace trc::TVM_space::types {
class trc_float : public trcobj {
public:
    double value {};

    void putline(FILE* out) override;

    ~trc_float() override;

    trc_float();

    explicit trc_float(const double& value);

    def::OBJ to_int() override;

    def::OBJ to_string() override;

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