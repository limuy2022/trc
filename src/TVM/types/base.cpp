/**
 * trcobj类型基类的函数实现
 * 部分无实际意义
 * 部分有报出相应错误的功能和职责
 */ 

#include "../../include/TVM/base.h"
#include "../../include/Error.h"

/* 生成固定的类型报错模板 */
#define ERROR_TYPE_MSG(type_name) "can't turn into "#type_name""
/* 生成固定的运算符报错模板 */
#define ERROR_OPER_MSG(type_name) "can't use "#type_name""

using namespace std;

trcobj::~trcobj() = default;
INTOBJ trcobj::to_int() {
    send_error(TypeError, ERROR_TYPE_MSG("int"));
}

STRINGOBJ trcobj::to_string() {
    send_error(TypeError, ERROR_TYPE_MSG("string"));
}

FLOATOBJ trcobj::to_float() {
    send_error(TypeError, ERROR_TYPE_MSG("float"));
}

INTOBJ trcobj::to_bool() {
    send_error(TypeError, ERROR_TYPE_MSG("bool"));
}

INTOBJ trcobj::operator<(OBJ value_i) {
    send_error(OperError, ERROR_OPER_MSG("<"));
}

INTOBJ trcobj::operator>(OBJ value_i) {
    send_error(OperError, ERROR_OPER_MSG(">"));
}

INTOBJ trcobj::operator<=(OBJ value_i) {
    send_error(OperError, ERROR_OPER_MSG("<="));
}

INTOBJ trcobj::operator>=(OBJ value_i) {
    send_error(OperError, ERROR_OPER_MSG(">="));
}

OBJ trcobj::operator+(OBJ value_i) {
    send_error(OperError, ERROR_OPER_MSG("+"));
}

OBJ trcobj::operator-(OBJ value_i) {
    send_error(OperError, ERROR_OPER_MSG("-"));
}

OBJ trcobj::operator*(OBJ value_i) {
    send_error(OperError, ERROR_OPER_MSG("*"));
}

OBJ trcobj::operator/(OBJ value_i) {
    send_error(OperError, ERROR_OPER_MSG("/"));
}

OBJ trcobj::operator%(OBJ value_i) {
    send_error(OperError, ERROR_OPER_MSG("%"));
}

OBJ trcobj::pow_(OBJ value_i) {
    send_error(OperError, ERROR_OPER_MSG("**"));
}

OBJ trcobj::zdiv(OBJ value_i) {
    send_error(OperError, ERROR_OPER_MSG("//"));
}

INTOBJ trcobj::operator!() {
    send_error(OperError, ERROR_OPER_MSG("!"));
}

INTOBJ trcobj::operator&&(OBJ value_i) {
    send_error(OperError, ERROR_OPER_MSG("&&"));
}

INTOBJ trcobj::operator||(OBJ value_i) {
    send_error(OperError, ERROR_OPER_MSG("||"));
}

void trcobj::delete_() {}
