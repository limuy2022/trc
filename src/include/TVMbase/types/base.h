/**
 * TVM虚拟机对象和数据对象的基类
 */

#pragma once

#include <ostream>
#include "base/trcdef.h"
#include "TVMbase/dll.h"

using namespace std;

namespace trc {
    namespace TVM_space {
        namespace types {
            class TRC_TVMbase_api trcobj {
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
            
                virtual void putline(ostream &out) = 0;
            
                virtual const int &gettype() = 0;
            };
        }
    }
}
