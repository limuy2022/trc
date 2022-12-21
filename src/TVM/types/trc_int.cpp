﻿#include <TVM/TRE.h>
#include <TVM/memory.h>
#include <TVM/types/trc_int.h>
#include <cmath>
#include <cstdio>

namespace trc::TVM_space::types {
const RUN_TYPE_TICK trc_int::type = RUN_TYPE_TICK::int_T;

void trc_int::putline(FILE* out) {
    if constexpr (sizeof(int) == sizeof(trc_int_t)) {
        fprintf(out, "%d", value);
    } else if constexpr (sizeof(long long) == sizeof(trc_int_t)) {
        fprintf(out, "%lld", value);
    } else if constexpr (sizeof(short) == sizeof(trc_int_t)) {
        fprintf(out, "%hd", value);
    } else if constexpr (sizeof(long) == sizeof(trc_int_t)) {
        fprintf(out, "%ld", value);
    } else {
        NOREACH("Unsupport int size.");
    }
}

trc_int::~trc_int() = default;

trc_int::trc_int() = default;

trc_int::trc_int(int value)
    : value(value) {
}

def::INTOBJ trc_int::operator==(def::OBJ value_i) {
    switch (value_i->gettype()) {
    case RUN_TYPE_TICK::int_T: {
        return (((def::INTOBJ)(value_i))->value != value ? TVM_share::false_
                                                         : TVM_share::true_);
    }
    case RUN_TYPE_TICK::float_T: {
        return (((def::FLOATOBJ)(value_i))->value != value ? TVM_share::false_
                                                           : TVM_share::true_);
    }
    default: {
        return nullptr;
    }
    }
}

def::INTOBJ trc_int::operator!=(def::OBJ value_i) {
    return (((def::INTOBJ)(value_i))->value == value ? TVM_share::false_
                                                     : TVM_share::true_);
}

def::INTOBJ trc_int::operator<(def::OBJ value_i) {
    return (value < ((def::INTOBJ)(value_i))->value ? TVM_share::true_
                                                    : TVM_share::false_);
}

def::INTOBJ trc_int::operator>(def::OBJ value_i) {
    return (value > ((def::INTOBJ)(value_i))->value ? TVM_share::true_
                                                    : TVM_share::false_);
}

def::INTOBJ trc_int::operator<=(def::OBJ value_i) {
    return (value <= ((def::INTOBJ)(value_i))->value ? TVM_share::true_
                                                     : TVM_share::false_);
}

def::INTOBJ trc_int::operator>=(def::OBJ value_i) {
    return (value >= ((def::INTOBJ)(value_i))->value ? TVM_share::true_
                                                     : TVM_share::false_);
}

def::OBJ trc_int::operator+(def::OBJ value_i) {
    return MALLOCINT(value + ((def::INTOBJ)(value_i))->value);
}

def::OBJ trc_int::operator-(def::OBJ value_i) {
    return MALLOCINT(value - ((def::INTOBJ)(value_i))->value);
}

def::OBJ trc_int::operator*(def::OBJ value_i) {
    return MALLOCINT(value * ((def::INTOBJ)(value_i))->value);
}

def::OBJ trc_int::operator/(def::OBJ value_i) {
    int second = ((def::INTOBJ)(value_i))->value;
    return (second ? MALLOCINT(value / second) : nullptr);
}

def::OBJ trc_int::operator%(def::OBJ value_i) {
    return MALLOCINT(value % ((def::INTOBJ)(value_i))->value);
}

def::OBJ trc_int::pow_(def::OBJ value_i) {
    return MALLOCINT(
        (int)pow((double)value, (double)(((def::INTOBJ)(value_i))->value)));
}

def::OBJ trc_int::zdiv(def::OBJ value_i) {
    return MALLOCINT(value / ((def::INTOBJ)(value_i))->value);
}

def::INTOBJ trc_int::operator!() {
    return (!value ? TVM_share::false_ : TVM_share::true_);
}

def::INTOBJ trc_int::operator&&(def::OBJ value_i) {
    return (value && ((def::INTOBJ)(value_i))->value ? TVM_share::true_
                                                     : TVM_share::false_);
}

def::INTOBJ trc_int::operator||(def::OBJ value_i) {
    return (value || ((def::INTOBJ)(value_i))->value ? TVM_share::true_
                                                     : TVM_share::false_);
}

def::OBJ trc_int::to_string() {
    return MALLOCSTRING(std::to_string(value).c_str());
}

def::OBJ trc_int::to_float() {
    return MALLOCFLOAT((double)value);
}

def::OBJ trc_int::to_bool() {
    return (value ? TVM_share::true_ : TVM_share::false_);
}

RUN_TYPE_TICK trc_int::gettype() {
    return type;
}
}
