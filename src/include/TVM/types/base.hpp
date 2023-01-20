/**
 * TVM虚拟机对象和数据对象的基类
 * 对于需要报出异常的函数，如果报出异常，则返回nullptr，需要在字节码处判断
 */

#pragma once

#include <TVM/TVMdef.hpp>
#include <base/trcdef.hpp>
#include <cstdio>

namespace trc::TVM_space::types {
/**
 * @brief
 * 所有数据类型的基类(包括用户自定义类，这些函数可以不实现)
 * 包含析构函数，比较函数，运算函数，
 * 逻辑运算符 注意：类不可实例化
 * 类型系统中的所有类都不是给项目本身使用的，
 * 仅仅服务于虚拟机，所以千万不要滥用对象和指针！
 */
class trcobj {
public:
    int refs = 0;
    // 第几代对象
    int gc_obj_ = 1;

    virtual ~trcobj() = default;

    virtual def::OBJ to_int();

    virtual def::OBJ to_string();

    virtual def::OBJ to_float();

    virtual def::OBJ to_bool();

    virtual def::INTOBJ operator==(def::OBJ value_i) = 0;

    virtual def::INTOBJ operator!=(def::OBJ value_i) = 0;

    virtual def::INTOBJ operator<(def::OBJ value_i);

    virtual def::INTOBJ operator>(def::OBJ value_i);

    virtual def::INTOBJ operator<=(def::OBJ value_i);

    virtual def::INTOBJ operator>=(def::OBJ value_i);

    virtual def::OBJ operator+(def::OBJ value_i);

    virtual def::OBJ operator-(def::OBJ value_i);

    virtual def::OBJ operator*(def::OBJ value_i);

    virtual def::OBJ operator/(def::OBJ value_i);

    virtual def::OBJ operator%(def::OBJ value_i);

    virtual def::OBJ pow_(def::OBJ value_i);

    virtual def::OBJ zdiv(def::OBJ value_i);

    virtual def::INTOBJ operator!();

    virtual def::INTOBJ operator&&(def::OBJ value_i);

    virtual def::INTOBJ operator||(def::OBJ value_i);

    virtual void putline(FILE* out) = 0;

    virtual RUN_TYPE_TICK gettype() = 0;
};
}
