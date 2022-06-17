/**
 * trcobj类型基类的函数实现
 * 部分无实际意义
 * 部分有报出相应错误的功能和职责
 */

#include <TVM/types/base.h>
#include <base/Error.h>
#include <language/language.h>
#include <cstring>

/**
 * 动态生成报错模板，为了能够在编译后通过dll切换语言
 */
static inline char* ERROR_TYPE_MSG(const char* type_name) {
    char* res = new char[strlen(language::TVM::type_change_error_msg)
        + strlen(type_name) + 1];
    strcpy(res, language::TVM::type_change_error_msg);
    strcat(res, type_name);
    return res;
}

static inline char* ERROR_OPER_MSG(const char* type_name) {
    char* res = new char[strlen(language::TVM::oper_not_def_error_msg)
        + strlen(type_name) + 1];
    strcpy(res, language::TVM::oper_not_def_error_msg);
    strcat(res, type_name);
    return res;
}

namespace trc::TVM_space::types {
trcobj::~trcobj() = default;

def::OBJ trcobj::to_int() {
    // char *error_msg = ERROR_TYPE_MSG("int");
    // error::send_error(error::TypeError,
    // error_msg); delete[] error_msg;
    return nullptr;
}

def::OBJ trcobj::to_string() {
    // char *error_msg = ERROR_TYPE_MSG("string");
    // error::send_error(error::TypeError,
    // error_msg); delete[] error_msg;
    return nullptr;
}

def::OBJ trcobj::to_float() {
    // char *error_msg = ERROR_TYPE_MSG("float");
    // error::send_error(error::TypeError,
    // error_msg); delete[] error_msg;
    return nullptr;
}

def::OBJ trcobj::to_bool() {
    // char *error_msg = ERROR_TYPE_MSG("bool");
    // error::send_error(error::TypeError,
    // error_msg); delete[] error_msg;
    return nullptr;
}

def::INTOBJ trcobj::operator<(def::OBJ value_i) {
    // char *error_msg = ERROR_OPER_MSG("<");
    // error::send_error(error::SyntaxError,
    // error_msg); delete[] error_msg;
    return nullptr;
}

def::INTOBJ trcobj::operator>(def::OBJ value_i) {
    // char *error_msg = ERROR_OPER_MSG(">");
    // error::send_error(error::SyntaxError,
    // error_msg); delete[] error_msg;
    return nullptr;
}

def::INTOBJ trcobj::operator<=(def::OBJ value_i) {
    // char *error_msg = ERROR_OPER_MSG("<=");
    // error::send_error(error::SyntaxError,
    // error_msg); delete[] error_msg;
    return nullptr;
}

def::INTOBJ trcobj::operator>=(def::OBJ value_i) {
    // char *error_msg = ERROR_OPER_MSG(">=");
    // error::send_error(error::SyntaxError,
    // error_msg); delete[] error_msg;
    return nullptr;
}

def::OBJ trcobj::operator+(def::OBJ value_i) {
    // char *error_msg = ERROR_OPER_MSG("+");
    // error::send_error(error::SyntaxError,
    // error_msg); delete[] error_msg;
    return nullptr;
}

def::OBJ trcobj::operator-(def::OBJ value_i) {
    // char *error_msg = ERROR_OPER_MSG("-");
    // error::send_error(error::SyntaxError,
    // error_msg); delete[] error_msg;
    return nullptr;
}

def::OBJ trcobj::operator*(def::OBJ value_i) {
    // char *error_msg = ERROR_OPER_MSG("*");
    // error::send_error(error::SyntaxError,
    // error_msg); delete[] error_msg;
    return nullptr;
}

def::OBJ trcobj::operator/(def::OBJ value_i) {
    // char *error_msg = ERROR_OPER_MSG("/");
    // error::send_error(error::SyntaxError,
    // error_msg); delete[] error_msg;
    return nullptr;
}

def::OBJ trcobj::operator%(def::OBJ value_i) {
    // char *error_msg = ERROR_OPER_MSG("%");
    // error::send_error(error::SyntaxError,
    // error_msg); delete[] error_msg;
    return nullptr;
}

def::OBJ trcobj::pow_(def::OBJ value_i) {
    // char *error_msg = ERROR_OPER_MSG("**");
    // error::send_error(error::SyntaxError,
    // error_msg); delete[] error_msg;
    return nullptr;
}

def::OBJ trcobj::zdiv(def::OBJ value_i) {
    // char *error_msg = ERROR_OPER_MSG("//");
    // error::send_error(error::SyntaxError,
    // error_msg); delete[] error_msg;
    return nullptr;
}

def::INTOBJ trcobj::operator!() {
    // char *error_msg = ERROR_OPER_MSG("!");
    // error::send_error(error::SyntaxError,
    // error_msg); delete[] error_msg;
    return nullptr;
}

def::INTOBJ trcobj::operator&&(def::OBJ value_i) {
    // char *error_msg = ERROR_OPER_MSG("&&");
    // error::send_error(error::SyntaxError,
    // error_msg); delete[] error_msg;
    return nullptr;
}

def::INTOBJ trcobj::operator||(def::OBJ value_i) {
    // char *error_msg = ERROR_OPER_MSG("||");
    // error::send_error(error::SyntaxError,
    // error_msg); delete[] error_msg;
    return nullptr;
}

void trcobj::delete_() {
}
}
