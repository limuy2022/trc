module;
#include <cstdio>
#include <obj_malloc.hpp>
#include <cmath>
#include <string>
module trc_float;
import TVMdef;
import trcdef;
import TRE;
import TVM.memory;
import trc_int;
import trc_string;

namespace trc::TVM_space::types {
const RUN_TYPE_TICK trc_float::type = RUN_TYPE_TICK::float_T;

void trc_float::putline(FILE* out) {
    fprintf(out, "%lf", value);
}

trc_float::~trc_float() = default;

trc_float::trc_float() = default;

trc_float::trc_float(const double& value)
    : value(value) {
}

def::INTOBJ trc_float::operator==(def::OBJ value_i) {
    return (((def::FLOATOBJ)(value_i))->value != value ? TVM_share::false_
                                                       : TVM_share::true_);
}

def::INTOBJ trc_float::operator!=(def::OBJ value_i) {
    return (((def::FLOATOBJ)(value_i))->value == value ? TVM_share::false_
                                                       : TVM_share::true_);
}

def::INTOBJ trc_float::operator<(def::OBJ value_i) {
    return (value < ((def::FLOATOBJ)(value_i))->value ? TVM_share::true_
                                                      : TVM_share::false_);
}

def::INTOBJ trc_float::operator>(def::OBJ value_i) {
    return (value > ((def::FLOATOBJ)(value_i))->value ? TVM_share::true_
                                                      : TVM_share::false_);
}

def::INTOBJ trc_float::operator<=(def::OBJ value_i) {
    return (value <= ((def::FLOATOBJ)(value_i))->value ? TVM_share::true_
                                                       : TVM_share::false_);
}

def::INTOBJ trc_float::operator>=(def::OBJ value_i) {
    return (value >= ((def::FLOATOBJ)(value_i))->value ? TVM_share::true_
                                                       : TVM_share::false_);
}

def::OBJ trc_float::operator+(def::OBJ value_i) {
    return MALLOCFLOAT(value + ((def::FLOATOBJ)(value_i))->value);
}

def::OBJ trc_float::operator-(def::OBJ value_i) {
    return MALLOCFLOAT(value - ((def::FLOATOBJ)(value_i))->value);
}

def::OBJ trc_float::operator*(def::OBJ value_i) {
    return MALLOCFLOAT(value * ((def::FLOATOBJ)(value_i))->value);
}

def::OBJ trc_float::operator/(def::OBJ value_i) {
    double second = ((def::FLOATOBJ)(value_i))->value;
    return second != 0 ? MALLOCFLOAT(value / second) : nullptr;
}

def::OBJ trc_float::operator%(def::OBJ value_i) {
    return MALLOCFLOAT(fmod(value, ((def::FLOATOBJ)(value_i))->value));
}

def::OBJ trc_float::pow_(def::OBJ value_i) {
    return MALLOCFLOAT(pow(value, ((def::FLOATOBJ)(value_i))->value));
}

def::OBJ trc_float::zdiv(def::OBJ value_i) {
    return MALLOCFLOAT((int)(value / ((def::FLOATOBJ)(value_i))->value));
}

def::INTOBJ trc_float::operator!() {
    return value == 0 ? TVM_share::true_ : TVM_share::false_;
}

def::INTOBJ trc_float::operator&&(def::OBJ value_i) {
    return value != 0 && ((def::FLOATOBJ)(value_i))->value != 0
        ? TVM_share::true_
        : TVM_share::false_;
}

def::INTOBJ trc_float::operator||(def::OBJ value_i) {
    return (value != 0 || ((def::FLOATOBJ)(value_i))->value != 0
            ? TVM_share::true_
            : TVM_share::false_);
}

def::OBJ trc_float::to_string() {
    return new trc_string(std::to_string(value));
}

def::OBJ trc_float::to_bool() {
    return (value != 0 ? TVM_share::true_ : TVM_share::false_);
}

def::OBJ trc_float::to_int() {
    return new trc_int((int)value);
}

RUN_TYPE_TICK trc_float::gettype() {
    return type;
}
}