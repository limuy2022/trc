#include <ostream>
#include <cmath>
#include <cstring>
#include "TVM/int.h"
#include "TVM/TRE.h"
#include "TVM/base.h"
#include "type.hpp"
#include "TVM/string.h"
#include "memory/mem.h"
#include "memory/objs_pool.hpp"

using namespace std;
using namespace memory;

const int trc_int::type;

void trc_int::putline(ostream& out) {
	out << value;
}

trc_int::~trc_int() = default;

trc_int::trc_int() = default;

trc_int::trc_int(int value) :
	value(value) {}

INTOBJ trc_int::operator==(OBJ value_i) {
	INTOBJ tick = (INTOBJ)(value_i);
	return (tick->value != value? TVM_share::false_ : TVM_share::true_);
}

INTOBJ trc_int::operator!=(OBJ value_i) {
	INTOBJ tick = (INTOBJ)(value_i);
	return (tick->value == value? TVM_share::false_ : TVM_share::true_);
}

INTOBJ trc_int::operator<(OBJ value_i) {
	INTOBJ tick = (INTOBJ)(value_i);
	return (value < tick->value?TVM_share::true_ : TVM_share::false_ );
}

INTOBJ trc_int::operator>(OBJ value_i) {
	INTOBJ tick = (INTOBJ)(value_i);
	return (value > tick->value?TVM_share::true_ : TVM_share::false_ );
}

INTOBJ trc_int::operator<=(OBJ value_i) {
	INTOBJ tick = (INTOBJ)(value_i);
	return (value <= tick->value?TVM_share::true_ : TVM_share::false_ );
}

INTOBJ trc_int::operator>=(OBJ value_i) {
	INTOBJ tick = (INTOBJ)(value_i);
	return (value >= tick->value?TVM_share::true_ : TVM_share::false_ );
}

OBJ trc_int::operator+(OBJ value_i) {
	INTOBJ tick = (INTOBJ)(value_i), tmp = MALLOCINT();
	tmp -> value = value + tick->value;
	return (OBJ)tmp;
}

OBJ trc_int::operator-(OBJ value_i) {
	INTOBJ tick = (INTOBJ)(value_i), tmp = MALLOCINT();
	tmp -> value = value - tick->value;
	return (OBJ)tmp;
}

OBJ trc_int::operator*(OBJ value_i) {
	INTOBJ tick = (INTOBJ)(value_i), tmp = MALLOCINT();
	tmp -> value = value * tick->value;
	return (OBJ)tmp;
}

OBJ trc_int::operator/(OBJ value_i) {
	INTOBJ tick = (INTOBJ)(value_i), tmp = MALLOCINT();
	tmp -> value = value / tick->value;
	return (OBJ)tmp;
}

OBJ trc_int::operator%(OBJ value_i) {
	INTOBJ tick = (INTOBJ)(value_i), tmp = MALLOCINT();
	tmp -> value = value % tick->value;
	return (OBJ)tmp;
}

OBJ trc_int::pow_(OBJ value_i) {
	INTOBJ tick = (INTOBJ)(value_i), tmp = MALLOCINT();
	tmp -> value = (int)pow((double)value, (double)(tick->value));
	return (OBJ)tmp;
}

OBJ trc_int::zdiv(OBJ value_i) {
	INTOBJ tick = (INTOBJ)(value_i), tmp = MALLOCINT();
	tmp -> value = value / tick->value;
	return (OBJ)tmp;
}

INTOBJ trc_int::operator!() {
	return (!value? TVM_share::false_: TVM_share::true_);
}

INTOBJ trc_int::operator&&(OBJ value_i) {
	INTOBJ tick = (INTOBJ)(value_i);
	return (value && value_i? TVM_share::true_: TVM_share::false_);
}

INTOBJ trc_int::operator||(OBJ value_i) {
	INTOBJ tick = (INTOBJ)(value_i);
	return (value || value_i? TVM_share::true_: TVM_share::false_);
}

STRINGOBJ trc_int::to_string() {
	auto tmp = MALLOCSTRING();
	strcpy(tmp -> value, std::to_string(value).c_str());
	return tmp;
}

FLOATOBJ trc_int::to_float() {
	auto tmp = MALLOCFLOAT();
	tmp -> value = value;
	return tmp;
}

INTOBJ trc_int::to_bool() {
    return (value? TVM_share::true_: TVM_share::false_);
}

const int& trc_int::gettype() {
	return type;
}

void trc_int::delete_() {
	value = 0;
}
