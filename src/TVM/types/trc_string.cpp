/**
 * TVM中的字符串类型
 * 注：自己实现的原因：
 * 1.本次实现的string不需要有太多的功能，而标准库中string内容太多，功能太繁杂
 * 2.作为一个类型，需要被gc统一管理，所以自己实现
 * 
 * 本次实现的string用法和标准库的差别很大，本次string只为TVM实现
 * 另外，trc_string类并不提供对char和char*进行操作的函数，仅支持trc_string本身
 */

#include <cstring>
#include <ostream>
#include <istream>
#include "TVM/string.h"
#include "TVM/TRE.h"
#include "Error.h"
#include "memory/mem.h"
#include "memory/objs_pool.hpp"
#include "share.h"
#include "type.hpp"

using namespace std;
using namespace TVM_share;

const int trc_string::type;

trc_string::trc_string(const trc_string &init):
    char_num(init.char_num)
 {
    value = (char*)(malloc(sizeof(char) * (char_num + 1)));
    if(value == NULL) {
        send_error(MemoryError, "can't get the memory from os.");
    }
    strcpy(value, init.value);
}

trc_string &trc_string::operator=(const string &init) {
    /**
     * 由于常量池，所以兼容string
     */ 
    char_num = init.length();
    value = (char*)(malloc(sizeof(char) * (char_num + 1)));
    if(value == NULL) {
        send_error(MemoryError, "can't get the memory from os.");
    }
    strcpy(value, init.c_str());
    return *this;
}

trc_string::trc_string(const string &init) :
    char_num(init.length())
{
    /**
     * 由于常量池，所以兼容string
     */ 
    value = (char*)(malloc(sizeof(char) * (char_num + 1)));
    if(value == NULL) {
        send_error(MemoryError, "can't get the memory from os.");
    }
    strcpy(value, init.c_str());
}

trc_string::~trc_string() {
    free(value);
}

trc_string::trc_string() {
    value = (char*)(malloc(sizeof(char)));
    if(value == NULL) {
        send_error(MemoryError, "can't get the memory from os.");
    }
    value[0] = '\0';
}

size_t trc_string::len() {
    return char_num;
}

OBJ trc_string::operator=(const trc_string& value_i) {
    if(&value_i != this) {
        set_realloc(value_i.char_num);
        strcpy(value, value_i.value);
    }
    return (OBJ)this;
}

OBJ trc_string::operator+(trcobj *value_i) {
    trc_string* tmp = MALLOCSTRING(*this);
    *tmp += value_i;
    return (OBJ)tmp;
}

OBJ trc_string::operator+=(trcobj* value_i) {
    trc_string *value_i_ = (STRINGOBJ)(value_i);
    set_realloc(value_i_->char_num + char_num);
    strcat(value, value_i_->value);
    return (OBJ)this;
}

char &trc_string::operator[](unsigned int index) {
    return value[index];
}

const char& trc_string::operator[](unsigned int index) const {
    return value[index];
}

void trc_string::putline(ostream &out_) {
    out_ << value;
}

istream& operator>>(istream &in_, trc_string &data_) {
    /**
     * 输入与标准库的方式不同，并不是读到空格停止，而是读到换行符停止
     */

    // 置空
    data_.set_realloc(0);
    char ch;
    for(;;) {
        ch = in_.get();
        if(ch == '\n') return in_;
        data_.set_realloc(data_.char_num + 1);
        data_.value[data_.char_num - 1] = ch;
    }
}

void trc_string::set_realloc(size_t num) {
    /**
     * 重新申请字符数，不包括\0
     */

    value = (char*)realloc(value, sizeof(char) * (num + 1));
    if(value == NULL)
        send_error(MemoryError, "can't get the memory from os.");
    char_num = num;
}

INTOBJ trc_string::operator==(trcobj *value_i) {
    STRINGOBJ tmp_ = (trc_string*)value_i;
    return (!strcmp(value, tmp_->value)? true_: false_);
}

INTOBJ trc_string::operator<(trcobj *value_i) {
    STRINGOBJ tmp_ = (trc_string*)value_i;
    return (strcmp(value, tmp_->value) < 0? true_: false_);
}

INTOBJ trc_string::operator>(trcobj *value_i) {
    STRINGOBJ tmp_ = (trc_string*)value_i;
    return (strcmp(value, tmp_->value) > 0? true_: false_);
}

INTOBJ trc_string::operator<=(trcobj *value_i) {
    STRINGOBJ tmp_ = (trc_string*)value_i;
    int tmp = strcmp(value, tmp_->value);
    return (tmp < 0 || !tmp? true_: false_);
}

INTOBJ trc_string::operator>=(trcobj * value_i) {
    STRINGOBJ tmp_ = (trc_string*)value_i;
    int tmp = strcmp(value, tmp_->value);
    return (tmp > 0 || !tmp? true_: false_);
}

INTOBJ trc_string::operator!=(trcobj * value_i) {
    STRINGOBJ tmp_ = (trc_string*)value_i;
    return (strcmp(value, tmp_->value)? true_: false_);
}

const char * trc_string::c_str() {
    return value;
}

OBJ trc_string::to_float() {
    return MALLOCFLOAT(atof(value));
}

OBJ trc_string::to_int() {
    return MALLOCINT(to_type<int>(value));
}

const int& trc_string::gettype() {
    return type;
}

OBJ trc_string::operator*(OBJ value_i) {
    if(value_i -> gettype() != int_TICK) {
        send_error(TypeError, "string can\'t * with " + type_int::int_name_s[value_i -> gettype()]);
    }
    INTOBJ tmp = (INTOBJ)(value_i);
    auto res = MALLOCSTRING();
    res -> set_realloc(char_num * (tmp -> value));
    for(int i = 0; i < tmp->value; ++i) {
        strcat(res -> value, value);
    }
    return (OBJ)res;
}

void trc_string::delete_() {
	set_realloc(1);
}
