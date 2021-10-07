#include <ostream>
#include <cmath>
#include "../../include/TVM/int.h"
#include "../../include/TVM/TRE.h"
#include "../../include/TVM/base.h"
#include "../../include/type.hpp"
#include "../../include/TVM/string.h"

using namespace std;

void trcint::putline(ostream& out) {
	out << value;
}

trcint::~trcint() = default;

trcint::trcint() = default;

trcint::trcint(int value) :
	value(value) {}

INTOBJ trcint::operator==(OBJ value_i) {
	INTOBJ tick = (INTOBJ)(value_i);
	return (tick->value != value? TVM_share::false_ : TVM_share::true_);
}

INTOBJ trcint::operator!=(OBJ value_i) {
	INTOBJ tick = (INTOBJ)(value_i);
	return (tick->value == value? TVM_share::false_ : TVM_share::true_);
}

INTOBJ trcint::operator<(OBJ value_i) {
	INTOBJ tick = (INTOBJ)(value_i);
	return (value < tick->value?TVM_share::true_ : TVM_share::false_ );
}

INTOBJ trcint::operator>(OBJ value_i) {
	INTOBJ tick = (INTOBJ)(value_i);
	return (value > tick->value?TVM_share::true_ : TVM_share::false_ );
}

INTOBJ trcint::operator<=(OBJ value_i) {
	INTOBJ tick = (INTOBJ)(value_i);
	return (value <= tick->value?TVM_share::true_ : TVM_share::false_ );
}

INTOBJ trcint::operator>=(OBJ value_i) {
	INTOBJ tick = (INTOBJ)(value_i);
	return (value >= tick->value?TVM_share::true_ : TVM_share::false_ );
}

OBJ trcint::operator+(OBJ value_i) {
	INTOBJ tick = (INTOBJ)(value_i);
	return (OBJ)(new trcint(value + tick->value));
}

OBJ trcint::operator-(OBJ value_i) {
	INTOBJ tick = (INTOBJ)(value_i);
	return (OBJ)(new trcint(value - tick->value));
}

OBJ trcint::operator*(OBJ value_i) {
	INTOBJ tick = (INTOBJ)(value_i);
	return (OBJ)(new trcint(value * tick->value));
}

OBJ trcint::operator/(OBJ value_i) {
	INTOBJ tick = (INTOBJ)(value_i);
	return (OBJ)new trcint(value / tick->value);
}

OBJ trcint::operator%(OBJ value_i) {
	INTOBJ tick = (INTOBJ)(value_i);
	return (OBJ)(new trcint(value % tick->value));
}

OBJ trcint::pow_(OBJ value_i) {
	INTOBJ tick = (INTOBJ)(value_i);
	return (OBJ)(new trcint((int)pow((double)value, (double)(tick->value))));
}

OBJ trcint::zdiv(OBJ value_i) {
	INTOBJ tick = (INTOBJ)(value_i);
	return (OBJ)(new trcint((int)(value / tick->value)));
}

INTOBJ trcint::operator!() {
	return (!value? TVM_share::false_: TVM_share::true_);
}

INTOBJ trcint::operator&&(OBJ value_i) {
	INTOBJ tick = (INTOBJ)(value_i);
	return (value && value_i? TVM_share::true_: TVM_share::false_);
}

INTOBJ trcint::operator||(OBJ value_i) {
	INTOBJ tick = (INTOBJ)(value_i);
	return (value || value_i? TVM_share::true_: TVM_share::false_);
}

STRINGOBJ trcint::to_string() {
	return new trc_string(std::to_string(value));
}

FLOATOBJ trcint::to_float() {

}

INTOBJ trcint::to_bool() {
    
}

INTOBJ trcint::to_int() {

}

int& trcint::gettype() {
	return type;
}

void trcint::delete_() {
	value = 0;
}
