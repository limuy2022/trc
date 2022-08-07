/**
 * TVM中的字符串类型
 * 注：自己实现的原因：
 * 1.本次实现的string不需要有太多的功能，而标准库中string内容太多，功能太繁杂
 * 2.作为一个类型，需要被gc统一管理，所以自己实现
 *
 * 本次实现的string用法和标准库的差别很大，本次string只为TVM实现
 * 另外，trc_string类并不提供对char和char*进行操作的函数，基本仅支持trc_string本身
 */

#include <TVM/TRE.h>
#include <TVM/TVMdef.h>
#include <TVM/memory.h>
#include <TVM/types/trc_flong.h>
#include <TVM/types/trc_string.h>
#include <base/io.hpp>
#include <base/trcdef.h>
#include <cstdio>
#include <cstring>

namespace trc::TVM_space::types {
using namespace TVM_share;
const RUN_TYPE_TICK trc_string::type = RUN_TYPE_TICK::string_T;

trc_string::trc_string(const trc_string& init)
    : char_num(init.char_num)
    , value((char*)(malloc(sizeof(char) * (char_num + 1)))) {
    strcpy(value, init.value);
}

trc_string& trc_string::operator=(const std::string& init) {
    char_num = init.length();
    set_realloc(char_num);
    strcpy(value, init.c_str());
    return *this;
}

trc_string::trc_string(const std::string& init)
    : char_num(init.length())
    , value((char*)(malloc(sizeof(char) * (char_num + 1)))) {
    strcpy(value, init.c_str());
}

trc_string::~trc_string() {
    free(value);
}

trc_string::trc_string()
    : value((char*)(malloc(sizeof(char)))) {
    *value = '\0';
}

size_t trc_string::len() {
    return char_num;
}

trc_string& trc_string::operator=(const trc_string& value_i) {
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

const char& trc_string::operator[](unsigned int index) const {
    return value[index];
}

void trc_string::putline(FILE* out) {
    fprintf(out, "%s", value);
}

void trc_string::in(FILE* in_) {
    // 置空
    free(value);
    char_num = 0;
    io::readstr(value, in_);
    char_num = strlen(value);
}

void trc_string::set_realloc(size_t num) {
    value = (char*)realloc(value, sizeof(char) * (num + 1));
    char_num = num;
}

def::INTOBJ trc_string::operator==(def::OBJ value_i) {
    return (!strcmp(value, ((def::STRINGOBJ)value_i)->value) ? true_ : false_);
}

def::INTOBJ trc_string::operator<(trcobj* value_i) {
    return (
        strcmp(value, ((def::STRINGOBJ)value_i)->value) < 0 ? true_ : false_);
}

def::INTOBJ trc_string::operator>(trcobj* value_i) {
    return (
        strcmp(value, ((def::STRINGOBJ)value_i)->value) > 0 ? true_ : false_);
}

def::INTOBJ trc_string::operator<=(trcobj* value_i) {
    int tmp = strcmp(value, ((def::STRINGOBJ)value_i)->value);
    return (tmp < 0 || !tmp ? true_ : false_);
}

def::INTOBJ trc_string::operator>=(trcobj* value_i) {
    int tmp = strcmp(value, ((def::STRINGOBJ)value_i)->value);
    return (tmp > 0 || !tmp ? true_ : false_);
}

def::INTOBJ trc_string::operator!=(trcobj* value_i) {
    return (strcmp(value, ((def::STRINGOBJ)value_i)->value) ? true_ : false_);
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
}
