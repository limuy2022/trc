/**
 * 由于c++本身并不支持大整数相加
 * 所以在这里通过trc_long来实现
 * 注意：该类重载了trc所支持的所有运算符
 * 包括+，-，*，/，==，！=等
 */

#pragma once

#include <TVM/TVMdef.h>
#include <TVMbase/library.h>
#include <TVMbase/types/base.h>
#include <base/trcdef.h>
#include <cstdio>
#include <string>

namespace trc::TVM_space::types {
class TRC_TVMbase_api trc_long : public trcobj {
public:
    trc_long(const std::string& a);

    trc_long();

    /**
     * @brief 将另一个类型值复制过来
     * @param a 另一个长整型
     */
    void operator=(def::OBJ a);

    def::OBJ operator+(def::OBJ a);

    def::OBJ operator-(def::OBJ a);

    def::OBJ operator*(def::OBJ v);

    def::OBJ operator/(def::OBJ);

    def::OBJ operator%(def::OBJ);

    def::OBJ pow(def::OBJ);

    def::OBJ zdiv(def::OBJ);

    def::INTOBJ operator!=(def::OBJ a);

    def::INTOBJ operator==(def::OBJ);

    def::INTOBJ operator<(def::OBJ);

    def::INTOBJ operator>(def::OBJ);

    def::INTOBJ operator<=(def::OBJ);

    def::INTOBJ operator>=(def::OBJ);

    void putline(FILE* out);

    RUN_TYPE_TICK gettype();

    def::INTOBJ operator!();

    def::INTOBJ operator&&(def::OBJ value_i);

    def::INTOBJ operator||(def::OBJ value_i);

    ~trc_long();

    def::OBJ to_string();

    def::OBJ to_float();

    def::OBJ to_bool();

    void delete_();

private:
    void set_alloc(size_t size_);

    const static RUN_TYPE_TICK type;

    // 第一位空出来，标识正负
    //之所以选择char，是因为每一位只需要保存一个数字，不需要int型
    char* value;

    // 当前大整数的使用长度，注意，符号位也包括在内，正数有默认的符号位
    // 0可以被标识为正数，也可以被标识为负数，不受影响
    size_t size = 2;
    // 当前已经申请的大小，因为为了性能考虑，每次申请都会多出一些字节，所以需要记录并区分
    size_t alloc_mem_size = 2;
};
}
