#include <ostream>
#include <cmath>
#include "type.hpp"
#include "share.h"
#include "TVM/float.h"
#include "TVM/int.h"
#include "TVM/TRE.h"

using namespace std;

const int trc_float::type;

void trc_float::putline(ostream& out) {
	out << value;
}

trc_float::~trc_float() = default;

trc_float::trc_float() = default;

trc_float::trc_float(const double &value) :
	value(value) {}

INTOBJ trc_float::operator==(OBJ value_i) {
	FLOATOBJ tick = (FLOATOBJ)(value_i);
	return (tick->value != value? TVM_share::false_ : TVM_share::true_);
}

INTOBJ trc_float::operator!=(OBJ value_i) {
	FLOATOBJ tick = (FLOATOBJ)(value_i);
	return (tick->value == value? TVM_share::false_ : TVM_share::true_);
}

INTOBJ trc_float::operator<(OBJ value_i) {
	FLOATOBJ tick = (FLOATOBJ)(value_i);
	return (value < tick->value?TVM_share::true_ : TVM_share::false_ );
}

INTOBJ trc_float::operator>(OBJ value_i) {
	FLOATOBJ tick = (FLOATOBJ)(value_i);
	return (value > tick->value?TVM_share::true_ : TVM_share::false_ );
}

INTOBJ trc_float::operator<=(OBJ value_i) {
	FLOATOBJ tick = (FLOATOBJ)(value_i);
	return (value <= tick->value?TVM_share::true_ : TVM_share::false_ );
}

INTOBJ trc_float::operator>=(OBJ value_i) {
	FLOATOBJ tick = (FLOATOBJ)(value_i);
	return (value >= tick->value?TVM_share::true_ : TVM_share::false_ );
}

OBJ trc_float::operator+(OBJ value_i) {
	FLOATOBJ tick = (FLOATOBJ)(value_i);
	return new trc_float(value + tick->value);
}

OBJ trc_float::operator-(OBJ value_i) {
	FLOATOBJ tick = (FLOATOBJ)(value_i);
	return new trc_float(value - tick->value);
}

OBJ trc_float::operator*(OBJ value_i) {
	FLOATOBJ tick = (FLOATOBJ)(value_i);
	return new trc_float(value * tick->value);
}

OBJ trc_float::operator/(OBJ value_i) {
	FLOATOBJ tick = (FLOATOBJ)(value_i);
	return new trc_float(value / tick->value);
}

OBJ trc_float::operator%(OBJ value_i) {
	FLOATOBJ tick = (FLOATOBJ)(value_i);
	return new trc_float(fmod(value, tick->value));
}

OBJ trc_float::pow_(OBJ value_i) {
	FLOATOBJ tick = (FLOATOBJ)(value_i);
	return new trc_float(pow(value, tick->value));
}

OBJ trc_float::zdiv(OBJ value_i) {
	FLOATOBJ tick = (FLOATOBJ)(value_i);
	return new trc_float((int)(value / tick->value));
}

INTOBJ trc_float::operator!() {
	return (!value? TVM_share::false_: TVM_share::true_);
}

INTOBJ trc_float::operator&&(OBJ value_i) {
	FLOATOBJ tick = (FLOATOBJ)(value_i);
	return (value && value_i? TVM_share::true_: TVM_share::false_);
}

INTOBJ trc_float::operator||(OBJ value_i) {
	FLOATOBJ tick = (FLOATOBJ)(value_i);
	return (value || value_i? TVM_share::true_: TVM_share::false_);
}

STRINGOBJ trc_float::to_string() {
	return new trc_string(std::to_string(value));
}

INTOBJ trc_float::to_bool() {
    return (value ? TVM_share::true_: TVM_share::false_);
}

INTOBJ trc_float::to_int() {
	return new trc_int(to_type<int>(value));
}

const int& trc_float::gettype() {
	return type;
}

void trc_float::delete_() {
	value = 0;
}
