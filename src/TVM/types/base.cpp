/**
 * trcobj类型基类的函数实现
 * 部分无实际意义
 * 部分有报出相应错误的功能和职责
 */

#include "TVM/base.h"
#include "Error.h"

/* 生成固定的类型报错模板 */
#define ERROR_TYPE_MSG(type_name) "can't turn into "#type_name""
/* 生成固定的运算符报错模板 */
#define ERROR_OPER_MSG(type_name) "can't use "#type_name""

using namespace std;

trcobj::~trcobj() = default;

OBJ trcobj::to_int() {
    send_error(TypeError, ERROR_TYPE_MSG("int"));
    return nullptr;
}

OBJ trcobj::to_string() {
    send_error(TypeError, ERROR_TYPE_MSG("string"));
    return nullptr;
}

OBJ trcobj::to_float() {
    send_error(TypeError, ERROR_TYPE_MSG("float"));
    return nullptr;
}

OBJ trcobj::to_bool() {
    send_error(TypeError, ERROR_TYPE_MSG("bool"));
    return nullptr;
}

INTOBJ trcobj::operator<(OBJ value_i) {
    send_error(SyntaxError, ERROR_OPER_MSG("<"));
    return (INTOBJ) nullptr;
}

INTOBJ trcobj::operator>(OBJ value_i) {
    send_error(SyntaxError, ERROR_OPER_MSG(">"));
    return (INTOBJ) nullptr;
}

INTOBJ trcobj::operator<=(OBJ value_i) {
    send_error(SyntaxError, ERROR_OPER_MSG("<="));
    return (INTOBJ) nullptr;
}

INTOBJ trcobj::operator>=(OBJ value_i) {
    send_error(SyntaxError, ERROR_OPER_MSG(">="));
    return (INTOBJ) nullptr;
}

OBJ trcobj::operator+(OBJ value_i) {
    send_error(SyntaxError, ERROR_OPER_MSG("+"));
    return nullptr;
}

OBJ trcobj::operator-(OBJ value_i) {
    send_error(SyntaxError, ERROR_OPER_MSG("-"));
    return nullptr;
}

OBJ trcobj::operator*(OBJ value_i) {
    send_error(SyntaxError, ERROR_OPER_MSG("*"));
    return nullptr;
}

OBJ trcobj::operator/(OBJ value_i) {
    send_error(SyntaxError, ERROR_OPER_MSG("/"));
    return nullptr;
}

OBJ trcobj::operator%(OBJ value_i) {
    send_error(SyntaxError, ERROR_OPER_MSG("%"));
    return nullptr;
}

OBJ trcobj::pow_(OBJ value_i) {
    send_error(SyntaxError, ERROR_OPER_MSG("**"));
    return nullptr;
}

OBJ trcobj::zdiv(OBJ value_i) {
    send_error(SyntaxError, ERROR_OPER_MSG("//"));
    return nullptr;
}

INTOBJ trcobj::operator!() {
    send_error(SyntaxError, ERROR_OPER_MSG("!"));
    return (INTOBJ) nullptr;
}

INTOBJ trcobj::operator&&(OBJ value_i) {
    send_error(SyntaxError, ERROR_OPER_MSG("&&"));
    return (INTOBJ) nullptr;
}

INTOBJ trcobj::operator||(OBJ value_i) {
    send_error(SyntaxError, ERROR_OPER_MSG("||"));
    return (INTOBJ) nullptr;
}

void trcobj::delete_() {}
