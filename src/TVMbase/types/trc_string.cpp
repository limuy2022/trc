/**
 * TVM中的字符串类型
 * 注：自己实现的原因：
 * 1.本次实现的string不需要有太多的功能，而标准库中string内容太多，功能太繁杂
 * 2.作为一个类型，需要被gc统一管理，所以自己实现
 *
 * 本次实现的string用法和标准库的差别很大，本次string只为TVM实现
 * 另外，trc_string类并不提供对char和char*进行操作的函数，基本仅支持trc_string本身
 */

#include "TVM/TVMdef.h"
#include "TVMbase/TRE.h"
#include "TVMbase/memory.h"
#include "TVMbase/types/flong.h"
#include "TVMbase/types/string.h"
#include "base/Error.h"
#include "base/memory/memory.h"
#include "base/trcdef.h"
#include "base/utils/type.hpp"
#include <cstdio>
#include <cstring>

namespace trc::TVM_space::types {
using namespace TVM_share;
const RUN_TYPE_TICK trc_string::type
    = RUN_TYPE_TICK::string_T;

trc_string::trc_string(const trc_string& init)
    : char_num(init.char_num)
    , value(
          (char*)(MALLOC(sizeof(char) * (char_num + 1)))) {
    strcpy(value, init.value);
}

trc_string& trc_string::operator=(const std::string& init) {
    /**
     * 由于常量池，所以兼容string
     */
    char_num = init.length();
    set_realloc(char_num);
    strcpy(value, init.c_str());
    return *this;
}

trc_string::trc_string(const std::string& init)
    : char_num(init.length())
    , value(
          (char*)(MALLOC(sizeof(char) * (char_num + 1)))) {
    /**
     * 由于常量池，所以兼容string
     */
    strcpy(value, init.c_str());
}

trc_string::~trc_string() {
    FREE(value, char_num + 1);
}

trc_string::trc_string()
    : value((char*)(MALLOC(sizeof(char)))) {
    *value = '\0';
}

size_t trc_string::len() {
    return char_num;
}

trc_string& trc_string::operator=(
    const trc_string& value_i) {
    if (&value_i != this) {
        set_realloc(value_i.char_num);
        strcpy(value, value_i.value);
    }
    return *this;
}

def::OBJ trc_string::operator+(def::OBJ value_i) {
    def::STRINGOBJ tmp = MALLOCSTRING(*this);
    *tmp += value_i;
    return tmp;
}

def::OBJ trc_string::operator+=(def::OBJ value_i) {
    auto value_i_ = (def::STRINGOBJ)(value_i);
    set_realloc(value_i_->char_num + char_num);
    strcat(value, value_i_->value);
    return this;
}

char& trc_string::operator[](unsigned int index) {
    return value[index];
}

const char& trc_string::operator[](
    unsigned int index) const {
    return value[index];
}

void trc_string::putline(FILE* out) {
    fprintf(out, "%s", value);
}

void trc_string::in(FILE* in_) {
    /**
     * 输入与标准库的方式不同，并不是读到空格停止，而是读到换行符停止
     */

    // 置空
    set_realloc(0);
    char ch;
    for (;;) {
        ch = fgetc(in_);
        if (ch == '\n')
            return;
        set_realloc(char_num + 1);
        value[char_num - 1] = ch;
    }
}

void trc_string::set_realloc(size_t num) {
    /**
     * 重新申请字符数，不包括\0
     */

    value = (char*)REALLOC(
        value, char_num + 1, sizeof(char) * (num + 1));
    char_num = num;
}

def::INTOBJ trc_string::operator==(def::OBJ value_i) {
    return (!strcmp(value, ((def::STRINGOBJ)value_i)->value)
            ? true_
            : false_);
}

def::INTOBJ trc_string::operator<(trcobj* value_i) {
    return (
        strcmp(value, ((def::STRINGOBJ)value_i)->value) < 0
            ? true_
            : false_);
}

def::INTOBJ trc_string::operator>(trcobj* value_i) {
    return (
        strcmp(value, ((def::STRINGOBJ)value_i)->value) > 0
            ? true_
            : false_);
}

def::INTOBJ trc_string::operator<=(trcobj* value_i) {
    int tmp
        = strcmp(value, ((def::STRINGOBJ)value_i)->value);
    return (tmp < 0 || !tmp ? true_ : false_);
}

def::INTOBJ trc_string::operator>=(trcobj* value_i) {
    int tmp
        = strcmp(value, ((def::STRINGOBJ)value_i)->value);
    return (tmp > 0 || !tmp ? true_ : false_);
}

def::INTOBJ trc_string::operator!=(trcobj* value_i) {
    return (strcmp(value, ((def::STRINGOBJ)value_i)->value)
            ? true_
            : false_);
}

const char* trc_string::c_str() const {
    return value;
}

def::OBJ trc_string::to_float() {
    return MALLOCFLOAT(atof(value));
}

def::OBJ trc_string::to_int() {
    return MALLOCINT(atoi(value));
}

RUN_TYPE_TICK trc_string::gettype() {
    return type;
}

def::OBJ trc_string::operator*(def::OBJ value_i) {
    if (value_i->gettype() != RUN_TYPE_TICK::int_T) {
        return nullptr;
    }
    int tmp = ((def::INTOBJ)(value_i))->value;
    auto res = MALLOCSTRING();
    res->set_realloc(char_num * tmp);
    for (int i = 0; i < tmp; ++i) {
        strcat(res->value, value);
    }
    return res;
}

void trc_string::delete_() {
    set_realloc(0);
}
}
