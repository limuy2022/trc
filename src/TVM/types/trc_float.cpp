#include <cmath>
#include "utils/type.hpp"
#include "TVM/float.h"
#include "TVM/int.h"
#include "TVM/TRE.h"
#include "memory/objs_pool.hpp"

using namespace std;

const int trc_float::type;

void trc_float::putline(ostream &out) {
    out << value;
}

trc_float::~trc_float() = default;

trc_float::trc_float() = default;

trc_float::trc_float(const double &value) :
        value(value) {}

INTOBJ trc_float::operator==(OBJ value_i) {
    return (((FLOATOBJ) (value_i))->value != value ? TVM_share::false_ : TVM_share::true_);
}

INTOBJ trc_float::operator!=(OBJ value_i) {
    return (((FLOATOBJ) (value_i))->value == value ? TVM_share::false_ : TVM_share::true_);
}

INTOBJ trc_float::operator<(OBJ value_i) {
    return (value < ((FLOATOBJ) (value_i))->value ? TVM_share::true_ : TVM_share::false_);
}

INTOBJ trc_float::operator>(OBJ value_i) {
    return (value > ((FLOATOBJ) (value_i))->value ? TVM_share::true_ : TVM_share::false_);
}

INTOBJ trc_float::operator<=(OBJ value_i) {
    return (value <= ((FLOATOBJ) (value_i))->value ? TVM_share::true_ : TVM_share::false_);
}

INTOBJ trc_float::operator>=(OBJ value_i) {
    return (value >= ((FLOATOBJ) (value_i))->value ? TVM_share::true_ : TVM_share::false_);
}

OBJ trc_float::operator+(OBJ value_i) {
    return MALLOCFLOAT(value + ((FLOATOBJ) (value_i))->value);
}

OBJ trc_float::operator-(OBJ value_i) {
    return MALLOCFLOAT(value - ((FLOATOBJ) (value_i))->value);
}

OBJ trc_float::operator*(OBJ value_i) {
    return MALLOCFLOAT(value * ((FLOATOBJ) (value_i))->value);
}

OBJ trc_float::operator/(OBJ value_i) {
    return MALLOCFLOAT(value / ((FLOATOBJ) (value_i))->value);
}

OBJ trc_float::operator%(OBJ value_i) {
    return MALLOCFLOAT(fmod(value, ((FLOATOBJ) (value_i))->value));
}

OBJ trc_float::pow_(OBJ value_i) {
    return MALLOCFLOAT(pow(value, ((FLOATOBJ) (value_i))->value));
}

OBJ trc_float::zdiv(OBJ value_i) {
    return MALLOCFLOAT((int) (value / ((FLOATOBJ) (value_i))->value));
}

INTOBJ trc_float::operator!() {
    return !value ? TVM_share::false_ : TVM_share::true_;
}

INTOBJ trc_float::operator&&(OBJ value_i) {
    return value && ((FLOATOBJ) (value_i))->value ? TVM_share::true_ : TVM_share::false_;
}

INTOBJ trc_float::operator||(OBJ value_i) {
    return (value || ((FLOATOBJ) (value_i)) -> value  ? TVM_share::true_ : TVM_share::false_);
}

OBJ trc_float::to_string() {
    return new trc_string(std::to_string(value));
}

OBJ trc_float::to_bool() {
    return (value ? TVM_share::true_ : TVM_share::false_);
}

OBJ trc_float::to_int() {
    return new trc_int((int) value);
}

const int &trc_float::gettype() {
    return type;
}
