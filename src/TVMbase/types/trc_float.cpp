#include <cmath>
#include "base/utils/type.hpp"
#include "TVMbase/types/float.h"
#include "TVMbase/types/int.h"
#include "TVMbase/types/flong.h"
#include "TVMbase/TRE.h"
#include "base/memory/objs_pool.hpp"
#include "TVMbase/memory.h"

using namespace std;

namespace trc {
    namespace TVM_space {
        namespace types {
            const int trc_float::type;

            void trc_float::putline(ostream &out) {
                out << value;
            }

            trc_float::~trc_float() = default;

            trc_float::trc_float() = default;

            trc_float::trc_float(const double &value) :
                    value(value) {}

            def::INTOBJ trc_float::operator==(def::OBJ value_i) {
                return (((def::FLOATOBJ) (value_i))->value != value ? TVM_share::false_ : TVM_share::true_);
            }

            def::INTOBJ trc_float::operator!=(def::OBJ value_i) {
                return (((def::FLOATOBJ) (value_i))->value == value ? TVM_share::false_ : TVM_share::true_);
            }

            def::INTOBJ trc_float::operator<(def::OBJ value_i) {
                return (value < ((def::FLOATOBJ) (value_i))->value ? TVM_share::true_ : TVM_share::false_);
            }

            def::INTOBJ trc_float::operator>(def::OBJ value_i) {
                return (value > ((def::FLOATOBJ) (value_i))->value ? TVM_share::true_ : TVM_share::false_);
            }

            def::INTOBJ trc_float::operator<=(def::OBJ value_i) {
                return (value <= ((def::FLOATOBJ) (value_i))->value ? TVM_share::true_ : TVM_share::false_);
            }

            def::INTOBJ trc_float::operator>=(def::OBJ value_i) {
                return (value >= ((def::FLOATOBJ) (value_i))->value ? TVM_share::true_ : TVM_share::false_);
            }

            def::OBJ trc_float::operator+(def::OBJ value_i) {
                return MALLOCFLOAT(value + ((def::FLOATOBJ) (value_i))->value);
            }

            def::OBJ trc_float::operator-(def::OBJ value_i) {
                return MALLOCFLOAT(value - ((def::FLOATOBJ) (value_i))->value);
            }

            def::OBJ trc_float::operator*(def::OBJ value_i) {
                return MALLOCFLOAT(value * ((def::FLOATOBJ) (value_i))->value);
            }

            def::OBJ trc_float::operator/(def::OBJ value_i) {
                return MALLOCFLOAT(value / ((def::FLOATOBJ) (value_i))->value);
            }

            def::OBJ trc_float::operator%(def::OBJ value_i) {
                return MALLOCFLOAT(fmod(value, ((def::FLOATOBJ) (value_i))->value));
            }

            def::OBJ trc_float::pow_(def::OBJ value_i) {
                return MALLOCFLOAT(pow(value, ((def::FLOATOBJ) (value_i))->value));
            }

            def::OBJ trc_float::zdiv(def::OBJ value_i) {
                return MALLOCFLOAT((int) (value / ((def::FLOATOBJ) (value_i))->value));
            }

            def::INTOBJ trc_float::operator!() {
                return !value ? TVM_share::false_ : TVM_share::true_;
            }

            def::INTOBJ trc_float::operator&&(def::OBJ value_i) {
                return value && ((def::FLOATOBJ) (value_i))->value ? TVM_share::true_ : TVM_share::false_;
            }

            def::INTOBJ trc_float::operator||(def::OBJ value_i) {
                return (value || ((def::FLOATOBJ) (value_i)) -> value  ? TVM_share::true_ : TVM_share::false_);
            }

            def::OBJ trc_float::to_string() {
                return new trc_string(std::to_string(value));
            }

            def::OBJ trc_float::to_bool() {
                return (value ? TVM_share::true_ : TVM_share::false_);
            }

            def::OBJ trc_float::to_int() {
                return new trc_int((int) value);
            }

            const int &trc_float::gettype() {
                return type;
            }
        }
    }
}
