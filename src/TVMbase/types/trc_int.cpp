#include <ostream>
#include <cmath>
#include "TVMbase/types/int.h"
#include "TVMbase/TRE.h"
#include "TVMbase/memory.h"
#include "TVMbase/types/flong.h"
#include "base/memory/objs_pool.hpp"

using namespace std;

namespace trc {
    namespace TVM_space {
        namespace types {
            const int trc_int::type;

            void trc_int::putline(ostream &out) {
                out << value;
            }

            trc_int::~trc_int() = default;

            trc_int::trc_int() = default;

            trc_int::trc_int(int value) :
                    value(value) {}

            def::INTOBJ trc_int::operator==(def::OBJ value_i) {
                return (((def::INTOBJ) (value_i))->value != value ? TVM_share::false_ : TVM_share::true_);
            }

            def::INTOBJ trc_int::operator!=(def::OBJ value_i) {
                return (((def::INTOBJ) (value_i))->value == value ? TVM_share::false_ : TVM_share::true_);
            }

            def::INTOBJ trc_int::operator<(def::OBJ value_i) {
                return (value < ((def::INTOBJ) (value_i))->value ? TVM_share::true_ : TVM_share::false_);
            }

            def::INTOBJ trc_int::operator>(def::OBJ value_i) {
                return (value > ((def::INTOBJ) (value_i))->value ? TVM_share::true_ : TVM_share::false_);
            }

            def::INTOBJ trc_int::operator<=(def::OBJ value_i) {
                return (value <= ((def::INTOBJ) (value_i))->value ? TVM_share::true_ : TVM_share::false_);
            }

            def::INTOBJ trc_int::operator>=(def::OBJ value_i) {
                return (value >= ((def::INTOBJ) (value_i))->value ? TVM_share::true_ : TVM_share::false_);
            }

            def::OBJ trc_int::operator+(def::OBJ value_i) {
                return MALLOCINT(value + ((def::INTOBJ) (value_i))->value);
            }

            def::OBJ trc_int::operator-(def::OBJ value_i) {
                return MALLOCINT(value - ((def::INTOBJ) (value_i))->value);
            }

            def::OBJ trc_int::operator*(def::OBJ value_i) {
                return MALLOCINT(value * ((def::INTOBJ) (value_i))->value);
            }

            def::OBJ trc_int::operator/(def::OBJ value_i) {
                return MALLOCINT(value / ((def::INTOBJ) (value_i))->value);
            }

            def::OBJ trc_int::operator%(def::OBJ value_i) {
                return MALLOCINT(value % ((def::INTOBJ) (value_i))->value);
            }

            def::OBJ trc_int::pow_(def::OBJ value_i) {
                return MALLOCINT((int) pow((double) value, (double) (((def::INTOBJ) (value_i))->value)));
            }

            def::OBJ trc_int::zdiv(def::OBJ value_i) {
                return MALLOCINT(value / ((def::INTOBJ) (value_i))->value);
            }

            def::INTOBJ trc_int::operator!() {
                return (!value ? TVM_share::false_ : TVM_share::true_);
            }

            def::INTOBJ trc_int::operator&&(def::OBJ value_i) {
                return (value && ((def::INTOBJ) (value_i)) -> value ? TVM_share::true_ : TVM_share::false_);
            }

            def::INTOBJ trc_int::operator||(def::OBJ value_i) {
                return (value || ((def::INTOBJ) (value_i)) -> value ? TVM_share::true_ : TVM_share::false_);
            }

            def::OBJ trc_int::to_string() {
                return MALLOCSTRING(std::to_string(value).c_str());
            }

            def::OBJ trc_int::to_float() {
                return MALLOCFLOAT((double) value);
            }

            def::OBJ trc_int::to_bool() {
                return (value ? TVM_share::true_ : TVM_share::false_);
            }

            const int &trc_int::gettype() {
                return type;
            }
        }
    }
}
