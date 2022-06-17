/**
 * 大整数运算
 */

#include "TVM/TRE.h"
#include "TVM/memory.h"
#include "TVM/types/long.h"
#include "base/memory/memory.h"
#include "base/memory/objs_pool.hpp"
#include <cstdio>
#include <cstring>
#include <string>

namespace trc::TVM_space::types {
using namespace TVM_share;
const RUN_TYPE_TICK trc_long::type = RUN_TYPE_TICK::trc_long_T;

trc_long::trc_long(const std::string& a) {
    size_t l_s = a.length();
    if (a[0] == '-') {
        size = l_s;
        value = (char*)(MALLOC(sizeof(char) * size));
        value[0] = 1;
        for (size_t i = 1; i < l_s; ++i) {
            value[i] = a[l_s - i] - '0';
        }
    } else {
        size = l_s + 1;
        value = (char*)(MALLOC(sizeof(char) * size));
        value[0] = 0;
        for (size_t i = 1; i <= l_s; ++i) {
            value[i] = a[l_s - i] - '0';
        }
    }
}

trc_long::trc_long()
    : value((char*)(MALLOC(
        sizeof(char) * 2))) { // 至少申请一个符号位，否则在realloc的过程中会卡死
    // 默认为+0
    value[0] = 0;
    value[1] = 0;
}

trc_long::~trc_long() {
    FREE(value, size);
}

void trc_long::operator=(def::OBJ a) {
    auto b = (def::LONGOBJ)(a);
    if (b != this) {
        set_alloc(b->size);
        memcpy(value, b->value, sizeof(char) * b->size);
    }
}

def::OBJ trc_long::operator+(def::OBJ b) {
    auto a = (def::LONGOBJ)(b);
    auto* res = MALLOCLONG();
    res->set_alloc(std::max(a->size, size) + 1);
    memcpy(res->value, value, size * sizeof(char));
    for (size_t i = 0; i < a->size; ++i) {
        res->value[i] += a->value[i];
        if (res->value[i] >= 10) {
            res->value[i] -= 10;
            ++(res->value[i + 1]);
        }
    }
    // 处理最后一位的运算
    int index = a->size;
    while (res->value[index] >= 10) {
        res->value[index] -= 10;
        res->value[index + 1]++;
        index++;
    }
    return res;
}

def::OBJ trc_long::operator-(def::OBJ b) {
    auto a = (def::LONGOBJ)(b);
    auto* res = MALLOCLONG();
    res->set_alloc(std::max(a->size, size));
    memcpy(res->value, value, size * sizeof(char));
    for (size_t i = 0; i < a->size; ++i) {
        res->value[i] -= a->value[i];
        if (res->value[i] < 0) {
            res->value[i] += 10;
            --(res->value[i + 1]);
        }
    }
    // 处理最后一位的运算
    int index = a->size;
    while (res->value[index] < 0) {
        res->value[index] += 10;
        res->value[index + 1]--;
        index++;
    }
    return res;
}

def::OBJ trc_long::operator*(def::OBJ b) {
    auto v = (def::LONGOBJ)(b);
    auto res = new trc_long;
    res->set_alloc(v->size + size - 1);
    int bit;
    for (size_t i = 1; i < size; ++i) {
        for (size_t j = 1; j < v->size; ++j) {
            bit = i + j - 1;
            res->value[bit] += value[i] * v->value[j];
            for (int k = bit;; ++k) {
                if (res->value[k] < 10)
                    break;
                res->value[k + 1] += res->value[k] / 10;
                res->value[k] %= 10;
            }
        }
    }
    return res;
}

void trc_long::set_alloc(size_t size_) {
    value = (char*)REALLOC(value, size, size_ * sizeof(char));
    /* 将多余的部分初始化为零 */
    if (size_ > size)
        memset(value + size, 0, (size_ - size) * sizeof(char));
    size = size_;
}

void trc_long::putline(FILE* out) {
    if (value[0])
        fputc('-', out);
    int i;
    for (i = size - 1; i > 1; --i)
        if (value[i])
            break;
    for (; i > 0; --i)
        fprintf(out, "%d", (int)value[i]);
}

def::OBJ trc_long::operator/(def::OBJ) {
    // TODO
    def::LONGOBJ res = MALLOCLONG();
    return res;
}

def::OBJ trc_long::operator%(def::OBJ) {
    // TODO
    def::LONGOBJ res = MALLOCLONG();
    return res;
}

def::OBJ trc_long::pow(def::OBJ) {
    // TODO
    def::LONGOBJ res = MALLOCLONG();
    return res;
}

def::OBJ trc_long::zdiv(def::OBJ) {
    // TODO
    def::LONGOBJ res = MALLOCLONG();
    return res;
}

def::INTOBJ trc_long::operator!=(def::OBJ b) {
    auto a = (def::LONGOBJ)(b);
    int min_ = std::min(size, a->size);
    for (int i = 1; i < min_; ++i)
        if (a->value[i] != value[i])
            return true_;
    return false_;
}

def::INTOBJ trc_long::operator==(def::OBJ b) {
    auto a = (def::LONGOBJ)(b);
    int min_ = std::min(size, a->size);
    for (int i = 1; i < min_; ++i)
        if (a->value[i] != value[i])
            return false_;
    return true_;
}

def::INTOBJ trc_long::operator<(def::OBJ b) {
    auto a = (def::LONGOBJ)(b);
    int min_ = std::min(size, a->size);
    for (int i = 1; i < min_; ++i)
        if (a->value[i] < value[i])
            return true_;
        else if (a->value[i] > value[i])
            return false_;
    return false_;
}

def::INTOBJ trc_long::operator>(def::OBJ b) {
    auto a = (def::LONGOBJ)(b);
    int min_ = std::min(size, a->size);
    for (int i = 1; i < min_; ++i)
        if (a->value[i] > value[i])
            return true_;
        else if (a->value[i] < value[i])
            return false_;
    return false_;
}

def::INTOBJ trc_long::operator<=(def::OBJ b) {
    auto a = (def::LONGOBJ)(b);
    int min_ = std::min(size, a->size);
    for (int i = 1; i < min_; ++i) {
        if (a->value[i] < value[i]) {
            return true_;
        } else if (a->value[i] > value[i]) {
            return false_;
        }
    }
    return true_;
}

def::INTOBJ trc_long::operator>=(def::OBJ b) {
    auto a = (def::LONGOBJ)(b);
    int min_ = std::min(size, a->size);
    for (int i = 1; i < min_; ++i) {
        if (a->value[i] > value[i]) {
            return true_;
        } else if (a->value[i] < value[i]) {
            return false_;
        }
    }
    return true_;
}

RUN_TYPE_TICK trc_long::gettype() {
    return type;
}

def::INTOBJ trc_long::operator!() {
    // todo
    return def::INTOBJ(nullptr);
}

def::INTOBJ trc_long::operator&&(def::OBJ value_i) {
    // todo
    return def::INTOBJ(nullptr);
}

def::INTOBJ trc_long::operator||(def::OBJ value_i) {
    // todo
    return def::INTOBJ(nullptr);
}

def::OBJ trc_long::to_string() {
    std::string tmp(value + 1, value + size);
    if (value[0])
        tmp = '-' + tmp;
    auto res = MALLOCSTRING(tmp);
    return res;
}

def::OBJ trc_long::to_float() {
    /**
     * 注意，这里是转成高精度小数
     */
    def::FLONGOBJ res = MALLOCFLONG();
    return res;
}

def::OBJ trc_long::to_bool() {
    return (size > 1 && !value[1] ? false_ : true_);
}

void trc_long::delete_() {
    set_alloc(1);
}
}
