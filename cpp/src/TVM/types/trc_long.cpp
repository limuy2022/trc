module;
#include <cassert>
#include <cmath>
#include <cstring>
#include <format>
#include <string>
module trc_long;
import TVM.memory;
import TRE;
import data;
import TVMdef;
import trcdef;
import TVM.memory;
import unreach;
import object;

namespace trc::TVM_space::types {
using namespace TVM_share;
const RUN_TYPE_TICK trc_long::type = RUN_TYPE_TICK::trc_long_T;
// 表示每一位实际存的数位
constexpr bit_type opt = 1000000000;
const unsigned int bitopt = utils::len(opt);

trc_long::trc_long(const std::string& a) {
    size_t l_s = a.length();
    size_t end_index;
    if (a[0] == '-') {
        used = size = (size_t)ceil((double(l_s - 1) * 1.0) / (bitopt - 1)) + 1;
        value = (bit_type*)(malloc(sizeof(bit_type) * size));
        value[0] = 1;
        end_index = 1;
    } else {
        used = size = (size_t)ceil((double)l_s / double(bitopt - 1)) + 1;
        value = (bit_type*)(malloc(sizeof(bit_type) * size));
        value[0] = 0;
        end_index = 0;
    }
    // 略过符号位，将之后的全部置空
    memset(value + 1, 0, (size - 1) * sizeof(bit_type));
    bit_type k = 1;
    size_t j = 1;
    for (size_t i = l_s - 1; i >= end_index; --i) {
        if (k == opt) {
            k = 1;
            j++;
        }
        value[j] += k * (a[i] - '0');
        k *= 10;
    }
    assert(j == used - 1);
}

trc_long::trc_long()
    : value((bit_type*)(malloc(size
        * sizeof(
            bit_type)))) { // 至少申请一个符号位，否则在realloc的过程中会卡死
    // 默认为+0
    value[0] = 0;
    value[1] = 0;
}

trc_long::trc_long(size_t size)
    : size(size)
    , value((bit_type*)malloc(sizeof(bit_type) * size)) {
    memset(value, 0, sizeof(bit_type) * size);
}

trc_long::~trc_long() {
    free(value);
}

void trc_long::cal_used_size() {
    // 不可使用无符号类型，否则将会永远死循环
    for (auto i = (long long)size; i >= 0; --i) {
        if (value[i] != 0) {
            used = i;
            return;
        }
    }
    unreach(std::format(
        "big num can't cal the used size.used:{}\nsize:{}", used, size));
}

trc_long& trc_long::operator=(def::OBJ a) {
    auto b = (def::LONGOBJ)(a);
    if (b != this) {
        set_alloc(b->size);
        memcpy(value, b->value, sizeof(bit_type) * b->size);
    }
    return *this;
}

def::OBJ trc_long::operator+(def::OBJ b) {
    auto a = (def::LONGOBJ)(b);
    // +1不是因为符号位，而是因为可能会进位
    auto* res = global_objs_pool->MALLOCLONG((std::max)(a->used, used) + 1);
    memcpy(res->value, value, used * sizeof(bit_type));
    for (size_t i = 1; i <= a->used; ++i) {
        res->value[i] += a->value[i];
        if (res->value[i] >= opt) {
            res->value[i] -= opt;
            ++(res->value[i + 1]);
        }
    }
    // 处理最后一位的运算
    size_t index = a->used;
    while (res->value[index] >= opt) {
        res->value[index] -= opt;
        res->value[index + 1]++;
        index++;
    }
    res->cal_used_size();
    return res;
}

def::OBJ trc_long::operator-(def::OBJ b) {
    auto a = (def::LONGOBJ)(b);
    auto* res = global_objs_pool->MALLOCLONG((std::max)(a->used, used));
    memcpy(res->value, value, used * sizeof(bit_type));
    for (size_t i = 0; i < a->used; ++i) {
        res->value[i] -= a->value[i];
        if (res->value[i] < 0) {
            res->value[i] += opt;
            --(res->value[i + 1]);
        }
    }
    // 处理最后一位的运算
    size_t index = a->used;
    while (res->value[index] < 0) {
        res->value[index] += opt;
        res->value[index + 1]--;
        index++;
    }
    res->cal_used_size();
    return res;
}

def::OBJ trc_long::operator*(def::OBJ b) {
    auto v = (def::LONGOBJ)(b);
    auto res = global_objs_pool->MALLOCLONG(v->used + used - 1);
    for (size_t i = 1; i < used; ++i) {
        for (size_t j = 1; j < v->used; ++j) {
            size_t bit = i + j - 1;
            res->value[bit] += value[i] * v->value[j];
            for (size_t k = bit;; ++k) {
                if (res->value[k] >= opt)
                    break;
                res->value[k + 1] += res->value[k] / opt;
                res->value[k] %= opt;
            }
        }
    }
    res->cal_used_size();
    return res;
}

void trc_long::set_alloc(size_t size_) {
    value = (bit_type*)realloc(value, size_ * sizeof(bit_type));
    size = size_;
}

void trc_long::putline(FILE* out) {
    if (value[0]) {
        fputc('-', out);
    }
    // 第一位不用补零，单独输出
    fprintf(out, "%lu", value[used - 1]);
    // 位压需要计算少了多少个零
    for (size_t i = used - 2; i > 0; --i) {
        for (unsigned int j = 1, zero_bit = bitopt - utils::len(value[i]);
             j < zero_bit; ++j) {
            fputc('0', out);
        }
        fprintf(out, "%lu", value[i]);
    }
}

def::OBJ trc_long::operator/(def::OBJ) {
    // TODO
    def::LONGOBJ res = global_objs_pool->MALLOCLONG();
    return res;
}

def::OBJ trc_long::operator%(def::OBJ) {
    // TODO
    def::LONGOBJ res = global_objs_pool->MALLOCLONG();
    return res;
}

def::OBJ trc_long::pow_(def::OBJ) {
    // TODO
    def::LONGOBJ res = global_objs_pool->MALLOCLONG();
    return res;
}

def::OBJ trc_long::zdiv(def::OBJ) {
    // TODO
    def::LONGOBJ res = global_objs_pool->MALLOCLONG();
    return res;
}

def::INTOBJ trc_long::operator!=(def::OBJ b) {
    // 首先比较位数
    auto a = (def::LONGOBJ)b;
    if (used != a->used) {
        return true_;
    }
    for (size_t i = 1; i <= used; ++i)
        if (a->value[i] != value[i])
            return true_;
    return false_;
}

def::INTOBJ trc_long::operator==(def::OBJ b) {
    auto a = (def::LONGOBJ)b;
    if (used != a->used) {
        return false_;
    }
    for (size_t i = 1; i <= used; ++i)
        if (a->value[i] != value[i])
            return false_;
    return true_;
}

def::INTOBJ trc_long::operator<(def::OBJ b) {
    auto a = (def::LONGOBJ)b;
    if (used > a->used) {
        return false_;
    } else if (used < a->used) {
        return true_;
    }
    for (size_t i = 1; i <= used; ++i)
        if (a->value[i] < value[i])
            return true_;
        else if (a->value[i] > value[i])
            return false_;
    return false_;
}

def::INTOBJ trc_long::operator>(def::OBJ b) {
    auto a = (def::LONGOBJ)b;
    if (used > a->used) {
        return true_;
    } else if (used < a->used) {
        return false_;
    }
    for (size_t i = 1; i <= used; ++i)
        if (a->value[i] > value[i])
            return true_;
        else if (a->value[i] < value[i])
            return false_;
    return false_;
}

def::INTOBJ trc_long::operator<=(def::OBJ b) {
    auto a = (def::LONGOBJ)b;
    if (used < a->used) {
        return true_;
    } else if (used > a->used) {
        return false_;
    }
    for (size_t i = 1; i <= used; ++i) {
        if (a->value[i] < value[i]) {
            return true_;
        } else if (a->value[i] > value[i]) {
            return false_;
        }
    }
    return true_;
}

def::INTOBJ trc_long::operator>=(def::OBJ b) {
    auto a = (def::LONGOBJ)b;
    if (used > a->used) {
        return true_;
    } else if (used < a->used) {
        return false_;
    }
    for (size_t i = 1; i <= used; ++i) {
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
    auto res = global_objs_pool->MALLOCSTRING(tmp);
    return res;
}

def::OBJ trc_long::to_float() {
    def::FLONGOBJ res = global_objs_pool->MALLOCFLONG();
    return res;
}

def::OBJ trc_long::to_bool() {
    return (size > 1 && !value[1] ? false_ : true_);
}
}