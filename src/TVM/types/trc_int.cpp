#include <ostream>
#include <cmath>
#include "TVM/int.h"
#include "TVM/TRE.h"
#include "memory/objs_pool.hpp"

using namespace std;
using namespace memory;

const int trc_int::type;

void trc_int::putline(ostream &out) {
    out << value;
}

trc_int::~trc_int() = default;

trc_int::trc_int() = default;

trc_int::trc_int(int value) :
        value(value) {}

INTOBJ trc_int::operator==(OBJ value_i) {
    return (((INTOBJ) (value_i))->value != value ? TVM_share::false_ : TVM_share::true_);
}

INTOBJ trc_int::operator!=(OBJ value_i) {
    return (((INTOBJ) (value_i))->value == value ? TVM_share::false_ : TVM_share::true_);
}

INTOBJ trc_int::operator<(OBJ value_i) {
    return (value < ((INTOBJ) (value_i))->value ? TVM_share::true_ : TVM_share::false_);
}

INTOBJ trc_int::operator>(OBJ value_i) {
    return (value > ((INTOBJ) (value_i))->value ? TVM_share::true_ : TVM_share::false_);
}

INTOBJ trc_int::operator<=(OBJ value_i) {
    return (value <= ((INTOBJ) (value_i))->value ? TVM_share::true_ : TVM_share::false_);
}

INTOBJ trc_int::operator>=(OBJ value_i) {
    return (value >= ((INTOBJ) (value_i))->value ? TVM_share::true_ : TVM_share::false_);
}

OBJ trc_int::operator+(OBJ value_i) {
    return MALLOCINT(value + ((INTOBJ) (value_i))->value);
}

OBJ trc_int::operator-(OBJ value_i) {
    return MALLOCINT(value - ((INTOBJ) (value_i))->value);
}

OBJ trc_int::operator*(OBJ value_i) {
    return MALLOCINT(value * ((INTOBJ) (value_i))->value);
}

OBJ trc_int::operator/(OBJ value_i) {
    return MALLOCINT(value / ((INTOBJ) (value_i))->value);
}

OBJ trc_int::operator%(OBJ value_i) {
    return MALLOCINT(value % ((INTOBJ) (value_i))->value);
}

OBJ trc_int::pow_(OBJ value_i) {
    return MALLOCINT((int) pow((double) value, (double) (((INTOBJ) (value_i))->value)));
}

OBJ trc_int::zdiv(OBJ value_i) {
    return MALLOCINT(value / ((INTOBJ) (value_i))->value);
}

INTOBJ trc_int::operator!() {
    return (!value ? TVM_share::false_ : TVM_share::true_);
}

INTOBJ trc_int::operator&&(OBJ value_i) {
    return (value && ((INTOBJ) (value_i)) -> value ? TVM_share::true_ : TVM_share::false_);
}

INTOBJ trc_int::operator||(OBJ value_i) {
    return (value || ((INTOBJ) (value_i)) -> value ? TVM_share::true_ : TVM_share::false_);
}

OBJ trc_int::to_string() {
    return MALLOCSTRING(std::to_string(value).c_str());
}

OBJ trc_int::to_float() {
    return MALLOCFLOAT((double) value);
}

OBJ trc_int::to_bool() {
    return (value ? TVM_share::true_ : TVM_share::false_);
}

const int &trc_int::gettype() {
    return type;
}
