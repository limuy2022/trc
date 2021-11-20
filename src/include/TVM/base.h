/**
 * TVM虚拟机对象和数据对象的基类
 */

#ifndef TRC_INCLUDE_TVM_BASE_H
#define TRC_INCLUDE_TVM_BASE_H

#include <ostream>
#include "trcdef.h"

using namespace std;

class trcobj {
    /**
     * 所有数据类型的基类(包括用户自定义类，这些函数可以不实现)
     * 包含析构函数，比较函数，运算函数， 逻辑运算符
     * 注意：类不可实例化
     * 类型系统中的所有类都不是给项目本身使用的，
     * 仅仅服务于虚拟机，所以千万不要滥用对象和指针！
     */

public:
    int refs = 0;
    // 第几代对象
    int gc_obj_ = 1;

    virtual ~trcobj();

    // 为了内存池的需要，由于内存池并不真正删除对象，所以需要提供一个"伪析构函数"
    virtual void delete_();

    virtual OBJ to_int();

    virtual OBJ to_string();

    virtual OBJ to_float();

    virtual OBJ to_bool();

    virtual INTOBJ operator==(OBJ value_i) = 0;

    virtual INTOBJ operator!=(OBJ value_i) = 0;

    virtual INTOBJ operator<(OBJ value_i);

    virtual INTOBJ operator>(OBJ value_i);

    virtual INTOBJ operator<=(OBJ value_i);

    virtual INTOBJ operator>=(OBJ value_i);

    virtual OBJ operator+(OBJ value_i);

    virtual OBJ operator-(OBJ value_i);

    virtual OBJ operator*(OBJ value_i);

    virtual OBJ operator/(OBJ value_i);

    virtual OBJ operator%(OBJ value_i);

    virtual OBJ pow_(OBJ value_i);

    virtual OBJ zdiv(OBJ value_i);

    virtual INTOBJ operator!();

    virtual INTOBJ operator&&(OBJ value_i);

    virtual INTOBJ operator||(OBJ value_i);

    virtual void putline(ostream &out) = 0;

    virtual const int &gettype() = 0;
};

#endif
