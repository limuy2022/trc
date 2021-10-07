/**
 * 大整数运算
 */

#include <string>
#include <cstring>
#include <ostream>
#include "../../include/TVM/bignum.h"
#include "../../include/Error.h"
#include "../../include/type.hpp"
#include "../../include/TVM/base.h"
#include "../../include/share.h"
#include "../../include/TVM/TRE.h"

using namespace std;
using namespace TVM_share;

#define NEW_BIGNUM_CHECK \
do{\
    if (NULL == value) {\
        send_error(MemoryError, "can't get the memory from os.");\
    }\
}while(0)

BigNum& BigNum::operator=(const string &a) {
    int l_s = a.length();
    if(a[0] == '-') {
        size = l_s;
        value = (char *)(malloc(sizeof(char) * size));
        NEW_BIGNUM_CHECK;
        value[0] = 1;
        for(int i = 1; i < l_s; ++i) {
            value[i] = a[l_s - i] - '0';
        }
        return *this;
    }
    size = l_s + 1;
    value = (char *)(malloc(sizeof(char) * size));
    NEW_BIGNUM_CHECK;
    value[0] = 0;
    for(int i = 1; i <= l_s; ++i) {
        value[i] = a[l_s - i] - '0';
    }
    return *this;
}

BigNum::BigNum() :
    value((char*)(malloc(sizeof(char)))) // 至少申请一个符号位，否则在realloc的过程中会卡死
{
    // 默认正数
    value = 0;
}

BigNum::~BigNum() {
    free(value);
}

OBJ BigNum::operator=(OBJ a) {
    LONGOBJ b = (LONGOBJ)(a);
    if(b != this) {
        set_alloc(b->size);
        memcpy(value, b->value, sizeof(char) * b->size);
    }
    return this;
}

OBJ BigNum::operator+(OBJ b) {
    LONGOBJ a = (LONGOBJ)(b);
    auto *res = new BigNum();
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

OBJ BigNum::operator-(OBJ b) {
    LONGOBJ a = (LONGOBJ)(b);
    auto *res = new BigNum;
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

OBJ BigNum::operator*(OBJ b) {
    LONGOBJ v = (LONGOBJ)(b);
    BigNum *res = new BigNum;
    res->set_alloc(v->size + size - 1);
    int bit;
    for (int i = 1; i < size; ++i) {
        for (int j = 1; j < v->size; ++j) {
            bit = i + j - 1;
            res->value[bit] += value[i] * v->value[j];
            for (int k = bit;; ++k) {
                if(res->value[k] < 10)
                    break;
                res->value[k + 1] += res->value[k] / 10;
                res->value[k] %= 10;
            }
        }
    }
    return res;
}

void BigNum::set_alloc(int size_) {
    value = (char *) realloc(value, size_ * sizeof(char));
    NEW_BIGNUM_CHECK;
    if (size_ > size)
        memset(value + size, 0, (size_ - size) * sizeof(char));
    size = size_;
}

void BigNum::putline(ostream &out_) {
    if (value[0])
        out_ << '-';
    int i;
    for (i = size - 1 ; i > 1; --i)
        if(value[i])
            break;
    for (; i > 0; --i)
        out_ << (int)value[i];
}

OBJ BigNum::operator/(OBJ) {

}

OBJ BigNum::operator%(OBJ) {

}

OBJ BigNum::pow(OBJ) {

}

OBJ BigNum::zdiv(OBJ) {

}

INTOBJ BigNum::operator!=(OBJ b) {
    LONGOBJ a = (LONGOBJ)(b);
    int min_ = min(size, a->size);
    for(int i = 1; i <min_; ++i)
        if(a->value[i] != value[i])
            return true_;
    return false_;
}

INTOBJ BigNum::operator==(OBJ b) {
    LONGOBJ a = (LONGOBJ)(b);
    int min_ = min(size, a->size);
    for(int i = 1; i < min_; ++i)
        if(a->value[i] != value[i])
            return false_;
    return true_;
}

INTOBJ BigNum::operator<(OBJ b) {
    LONGOBJ a = (LONGOBJ)(b);
    int min_ = min(size, a->size);
    for(int i = 1; i <min_; ++i)
        if(a->value[i] <  value[i])
            return true_;
        else if(a->value[i] >  value[i])
            return false_;
    return false_;
}

INTOBJ BigNum::operator>(OBJ b) {
    LONGOBJ a = (LONGOBJ)(b);
    int min_ = min(size, a->size);
    for(int i = 1; i <min_; ++i)
        if(a->value[i] >  value[i])
            return true_;
        else if(a->value[i] <  value[i])
            return false_;
    return false_;
}

INTOBJ BigNum::operator<=(OBJ b) {
    LONGOBJ a = (LONGOBJ)(b);
    int min_ = min(size, a->size);
    for(int i = 1; i <min_; ++i) {
        if(a->value[i] <  value[i]) {
            return true_;
        } else if(a->value[i] >  value[i]) {
            return false_;
        }
    }
    return true_;
}

INTOBJ BigNum::operator>=(OBJ b) {
    LONGOBJ a = (LONGOBJ)(b);
    int min_ = min(size, a->size);
    for(int i = 1; i <min_; ++i) {
        if(a->value[i] >  value[i]) {
            return true_;
        } else if(a->value[i] <  value[i]) {
            return false_;
        }
    }
    return true_;
}

int& BigNum::gettype() {
    return type;
}

INTOBJ BigNum::operator!() {

}

INTOBJ BigNum::operator&&(OBJ value_i) {

}

INTOBJ BigNum::operator||(OBJ value_i) {

}

INTOBJ BigNum::to_int() {

}

STRINGOBJ BigNum::to_string() {

}

FLOATOBJ BigNum::to_float() {

}

INTOBJ BigNum::to_bool() {

}

void BigNum::delete_() {
    set_alloc(1);
}
