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

// 作为返回值返回，尽管会因为报错直接退出函数，根本不需要返回值，
// 但有些编译器会因为没有返回值无法编译通过，为了能够更好的跨平台，创建此变量
const static OBJ return_value = nullptr;

trcobj::~trcobj() = default;

OBJ trcobj::to_int() {
    send_error(TypeError, ERROR_TYPE_MSG("int"));
    return return_value;
}

OBJ trcobj::to_string() {
    send_error(TypeError, ERROR_TYPE_MSG("string"));
    return return_value;
}

OBJ trcobj::to_float() {
    send_error(TypeError, ERROR_TYPE_MSG("float"));
    return return_value;
}

OBJ trcobj::to_bool() {
    send_error(TypeError, ERROR_TYPE_MSG("bool"));
    return return_value;
}

INTOBJ trcobj::operator<(OBJ value_i) {
    send_error(SyntaxError, ERROR_OPER_MSG("<"));
    return (INTOBJ) return_value;
}

INTOBJ trcobj::operator>(OBJ value_i) {
    send_error(SyntaxError, ERROR_OPER_MSG(">"));
    return (INTOBJ) return_value;
}

INTOBJ trcobj::operator<=(OBJ value_i) {
    send_error(SyntaxError, ERROR_OPER_MSG("<="));
    return (INTOBJ) return_value;
}

INTOBJ trcobj::operator>=(OBJ value_i) {
    send_error(SyntaxError, ERROR_OPER_MSG(">="));
    return (INTOBJ) return_value;
}

OBJ trcobj::operator+(OBJ value_i) {
    send_error(SyntaxError, ERROR_OPER_MSG("+"));
    return return_value;
}

OBJ trcobj::operator-(OBJ value_i) {
    send_error(SyntaxError, ERROR_OPER_MSG("-"));
    return return_value;
}

OBJ trcobj::operator*(OBJ value_i) {
    send_error(SyntaxError, ERROR_OPER_MSG("*"));
    return return_value;
}

OBJ trcobj::operator/(OBJ value_i) {
    send_error(SyntaxError, ERROR_OPER_MSG("/"));
    return return_value;
}

OBJ trcobj::operator%(OBJ value_i) {
    send_error(SyntaxError, ERROR_OPER_MSG("%"));
    return return_value;
}

OBJ trcobj::pow_(OBJ value_i) {
    send_error(SyntaxError, ERROR_OPER_MSG("**"));
    return return_value;
}

OBJ trcobj::zdiv(OBJ value_i) {
    send_error(SyntaxError, ERROR_OPER_MSG("//"));
    return return_value;
}

INTOBJ trcobj::operator!() {
    send_error(SyntaxError, ERROR_OPER_MSG("!"));
    return (INTOBJ) return_value;
}

INTOBJ trcobj::operator&&(OBJ value_i) {
    send_error(SyntaxError, ERROR_OPER_MSG("&&"));
    return (INTOBJ) return_value;
}

INTOBJ trcobj::operator||(OBJ value_i) {
    send_error(SyntaxError, ERROR_OPER_MSG("||"));
    return (INTOBJ) return_value;
}

void trcobj::delete_() {}
