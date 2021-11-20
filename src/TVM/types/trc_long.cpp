/**
 * 大整数运算
 */

#include <string>
#include <cstring>
#include <ostream>
#include "TVM/long.h"
#include "TVM/TRE.h"
#include "memory/objs_pool.hpp"

using namespace std;
using namespace TVM_share;

const int trc_long::type;

trc_long::trc_long(const string &a) {
    int l_s = a.length();
    if (a[0] == '-') {
        size = l_s;
        value = (char *) (MALLOC(sizeof(char) * size));
        value[0] = 1;
        for (int i = 1; i < l_s; ++i) {
            value[i] = a[l_s - i] - '0';
        }
        return;
    }
    size = l_s + 1;
    value = (char *) (MALLOC(sizeof(char) * size));
    value[0] = 0;
    for (int i = 1; i <= l_s; ++i) {
        value[i] = a[l_s - i] - '0';
    }
}

trc_long::trc_long() :
        value((char *) (MALLOC(sizeof(char)))) // 至少申请一个符号位，否则在realloc的过程中会卡死
{
    // 默认正数
    *value = 0;
}

trc_long::~trc_long() {
    FREE(value, size);
}

OBJ trc_long::operator=(OBJ a) {
    auto b = (LONGOBJ) (a);
    if (b != this) {
        set_alloc(b->size);
        memcpy(value, b->value, sizeof(char) * b->size);
    }
    return this;
}

OBJ trc_long::operator+(OBJ b) {
    auto a = (LONGOBJ) (b);
    auto *res = MALLOCLONG();
    res->set_alloc(max(a->size, size) + 1);
    memcpy(res->value, value, size * sizeof(char));
    for (int i = 0; i < a->size; ++i) {
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

OBJ trc_long::operator-(OBJ b) {
    auto a = (LONGOBJ) (b);
    auto *res = MALLOCLONG();
    res->set_alloc(max(a->size, size));
    memcpy(res->value, value, size * sizeof(char));
    for (int i = 0; i < a->size; ++i) {
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

OBJ trc_long::operator*(OBJ b) {
    auto v = (LONGOBJ) (b);
    auto res = new trc_long;
    res->set_alloc(v->size + size - 1);
    int bit;
    for (int i = 1; i < size; ++i) {
        for (int j = 1; j < v->size; ++j) {
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

void trc_long::set_alloc(int size_) {
    value = (char *) REALLOC(value, size, size_ * sizeof(char));
    /* 将多余的部分初始化为零 */
    if (size_ > size)
        memset(value + size, 0, (size_ - size) * sizeof(char));
    size = size_;
}

void trc_long::putline(ostream &out_) {
    if (value[0])
        out_ << '-';
    int i;
    for (i = size - 1; i > 1; --i)
        if (value[i])
            break;
    for (; i > 0; --i)
        out_ << (int) value[i];
}

OBJ trc_long::operator/(OBJ) {
    // TODO
    LONGOBJ res = MALLOCLONG();
    return res;
}

OBJ trc_long::operator%(OBJ) {
    // TODO
    LONGOBJ res = MALLOCLONG();
    return res;
}

OBJ trc_long::pow(OBJ) {
    // TODO
    LONGOBJ res = MALLOCLONG();
    return res;
}

OBJ trc_long::zdiv(OBJ) {
    // TODO
    LONGOBJ res = MALLOCLONG();
    return res;
}

INTOBJ trc_long::operator!=(OBJ b) {
    auto a = (LONGOBJ) (b);
    int min_ = min(size, a->size);
    for (int i = 1; i < min_; ++i)
        if (a->value[i] != value[i])
            return true_;
    return false_;
}

INTOBJ trc_long::operator==(OBJ b) {
    auto a = (LONGOBJ) (b);
    int min_ = min(size, a->size);
    for (int i = 1; i < min_; ++i)
        if (a->value[i] != value[i])
            return false_;
    return true_;
}

INTOBJ trc_long::operator<(OBJ b) {
    auto a = (LONGOBJ) (b);
    int min_ = min(size, a->size);
    for (int i = 1; i < min_; ++i)
        if (a->value[i] < value[i])
            return true_;
        else if (a->value[i] > value[i])
            return false_;
    return false_;
}

INTOBJ trc_long::operator>(OBJ b) {
    auto a = (LONGOBJ) (b);
    int min_ = min(size, a->size);
    for (int i = 1; i < min_; ++i)
        if (a->value[i] > value[i])
            return true_;
        else if (a->value[i] < value[i])
            return false_;
    return false_;
}

INTOBJ trc_long::operator<=(OBJ b) {
    auto a = (LONGOBJ) (b);
    int min_ = min(size, a->size);
    for (int i = 1; i < min_; ++i) {
        if (a->value[i] < value[i]) {
            return true_;
        } else if (a->value[i] > value[i]) {
            return false_;
        }
    }
    return true_;
}

INTOBJ trc_long::operator>=(OBJ b) {
    auto a = (LONGOBJ) (b);
    int min_ = min(size, a->size);
    for (int i = 1; i < min_; ++i) {
        if (a->value[i] > value[i]) {
            return true_;
        } else if (a->value[i] < value[i]) {
            return false_;
        }
    }
    return true_;
}

const int &trc_long::gettype() {
    return type;
}

INTOBJ trc_long::operator!() {
    // todo
    return INTOBJ();
}

INTOBJ trc_long::operator&&(OBJ value_i) {
    // todo
    return INTOBJ();
}

INTOBJ trc_long::operator||(OBJ value_i) {
    // todo
    return INTOBJ();
}

OBJ trc_long::to_string() {
    string tmp(value + 1, value + size);
    if (value[0])
        tmp = '-' + tmp;
    auto res = MALLOCSTRING(tmp);
    return res;
}

OBJ trc_long::to_float() {
    /**
     * 注意，这里是转成高精度小数
     */
    FLONGOBJ res = MALLOCFLONG();
    return res;
}

OBJ trc_long::to_bool() {
    return (!value[0] && size > 1 && !value[1] ? false_ : true_);
}

void trc_long::delete_() {
    set_alloc(1);
}
