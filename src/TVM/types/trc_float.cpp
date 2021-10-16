#include <ostream>
#include <cmath>
#include "../../include/type.hpp"
#include "../../include/share.h"
#include "../../include/TVM/float.h"
#include "../../include/TVM/int.h"
#include "../../include/TVM/TRE.h"

using namespace std;

void trcfloat::putline(ostream& out) {
	out << value;
}

trcfloat::~trcfloat() = default;

trcfloat::trcfloat() = default;

trcfloat::trcfloat(const double &value) :
	value(value) {}

INTOBJ trcfloat::operator==(OBJ value_i) {
	FLOATOBJ tick = (FLOATOBJ)(value_i);
	return (tick->value != value? TVM_share::false_ : TVM_share::true_);
}

INTOBJ trcfloat::operator!=(OBJ value_i) {
	FLOATOBJ tick = (FLOATOBJ)(value_i);
	return (tick->value == value? TVM_share::false_ : TVM_share::true_);
}

INTOBJ trcfloat::operator<(OBJ value_i) {
	FLOATOBJ tick = (FLOATOBJ)(value_i);
	return (value < tick->value?TVM_share::true_ : TVM_share::false_ );
}

INTOBJ trcfloat::operator>(OBJ value_i) {
	FLOATOBJ tick = (FLOATOBJ)(value_i);
	return (value > tick->value?TVM_share::true_ : TVM_share::false_ );
}

INTOBJ trcfloat::operator<=(OBJ value_i) {
	FLOATOBJ tick = (FLOATOBJ)(value_i);
	return (value <= tick->value?TVM_share::true_ : TVM_share::false_ );
}

INTOBJ trcfloat::operator>=(OBJ value_i) {
	FLOATOBJ tick = (FLOATOBJ)(value_i);
	return (value >= tick->value?TVM_share::true_ : TVM_share::false_ );
}

OBJ trcfloat::operator+(OBJ value_i) {
	FLOATOBJ tick = (FLOATOBJ)(value_i);
	return new trcfloat(value + tick->value);
}

OBJ trcfloat::operator-(OBJ value_i) {
	FLOATOBJ tick = (FLOATOBJ)(value_i);
	return new trcfloat(value - tick->value);
}

OBJ trcfloat::operator*(OBJ value_i) {
	FLOATOBJ tick = (FLOATOBJ)(value_i);
	return new trcfloat(value * tick->value);
}

OBJ trcfloat::operator/(OBJ value_i) {
	FLOATOBJ tick = (FLOATOBJ)(value_i);
	return new trcfloat(value / tick->value);
}

OBJ trcfloat::operator%(OBJ value_i) {
	FLOATOBJ tick = (FLOATOBJ)(value_i);
	return new trcfloat(fmod(value, tick->value));
}

OBJ trcfloat::pow_(OBJ value_i) {
	FLOATOBJ tick = (FLOATOBJ)(value_i);
	return new trcfloat(pow(value, tick->value));
}

OBJ trcfloat::zdiv(OBJ value_i) {
	FLOATOBJ tick = (FLOATOBJ)(value_i);
	return new trcfloat((int)(value / tick->value));
}

INTOBJ trcfloat::operator!() {
	return (!value? TVM_share::false_: TVM_share::true_);
}

INTOBJ trcfloat::operator&&(OBJ value_i) {
	FLOATOBJ tick = (FLOATOBJ)(value_i);
	return (value && value_i? TVM_share::true_: TVM_share::false_);
}

INTOBJ trcfloat::operator||(OBJ value_i) {
	FLOATOBJ tick = (FLOATOBJ)(value_i);
	return (value || value_i? TVM_share::true_: TVM_share::false_);
}

STRINGOBJ trcfloat::to_string() {
	return new trc_string(std::to_string(value));
}

INTOBJ trcfloat::to_bool() {
    return (value ? TVM_share::true_: TVM_share::false_);
}

INTOBJ trcfloat::to_int() {
	return new trcint(to_type<int>(value));
}

int& trcfloat::gettype() {
	return type;
}

void trcfloat::delete_() {
	value = 0;
}
