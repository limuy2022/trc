module;
#include <string>
#include <cstdio>
module trc_flong;
import TVMdef;
import base;
import trcdef;

namespace trc::TVM_space::types {
const RUN_TYPE_TICK trc_flong::type = RUN_TYPE_TICK::float_T;

trc_flong::trc_flong(const std::string&) {
}

trc_flong::trc_flong(double init_data) {
}

trc_flong::trc_flong() {
}

trc_flong::~trc_flong() {
}

void trc_flong::putline(FILE* out) {
}

RUN_TYPE_TICK trc_flong::gettype() {
    return type;
}

def::INTOBJ trc_flong::operator==(def::OBJ value_i) {
    return def::INTOBJ(nullptr);
}

def::INTOBJ trc_flong::operator!=(def::OBJ value_i) {
    return def::INTOBJ(nullptr);
}

void trc_flong::set_realloc(size_t num) {
}
}
