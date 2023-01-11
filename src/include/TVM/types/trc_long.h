/**
 * 由于c++本身并不支持大整数相加
 * 所以在这里通过trc_long来实现
 * 注意：该类重载了trc所支持的所有运算符
 * 包括+，-，*，/，==，！=等
 */

#pragma once

#include <TVM/TVMdef.h>

#include <TVM/types/base.h>
#include <base/trcdef.h>
#include <cstdint>
#include <cstdio>
#include <string>

// 整型和长整型的分界，大于十的就是长整型
#define INT_LONGINT_LINE 9

namespace trc::TVM_space::types {
// 高精度数每一位的类型
typedef uint64_t bit_type;

class trc_long : public trcobj {
public:
    explicit trc_long(const std::string& a);

    trc_long();

    /**
     * @param size 表示申请多少个位作为预留
     * @warning 不会自动添加符号位
     */
    explicit trc_long(size_t size);

    /**
     * @brief 将另一个类型值复制过来
     * @param a 另一个长整型
     */
    trc_long& operator=(def::OBJ a);

    def::OBJ operator+(def::OBJ a) override;

    def::OBJ operator-(def::OBJ a) override;

    def::OBJ operator*(def::OBJ v) override;

    def::OBJ operator/(def::OBJ) override;

    def::OBJ operator%(def::OBJ) override;

    def::OBJ pow_(def::OBJ) override;

    def::OBJ zdiv(def::OBJ) override;

    def::INTOBJ operator!=(def::OBJ a) override;

    def::INTOBJ operator==(def::OBJ) override;

    def::INTOBJ operator<(def::OBJ) override;

    def::INTOBJ operator>(def::OBJ) override;

    def::INTOBJ operator<=(def::OBJ) override;

    def::INTOBJ operator>=(def::OBJ) override;

    void putline(FILE* out) override;

    RUN_TYPE_TICK gettype() override;

    def::INTOBJ operator!() override;

    def::INTOBJ operator&&(def::OBJ value_i) override;

    def::INTOBJ operator||(def::OBJ value_i) override;

    ~trc_long() override;

    def::OBJ to_string() override;

    /**
     * @warning 这里是转成高精度小数
     */
    def::OBJ to_float() override;

    def::OBJ to_bool() override;

    /**
     * @brief 将长度修复到正确的
     */
    void cal_used_size();

private:
    void set_alloc(size_t size_);

    const static RUN_TYPE_TICK type;

    // 当前已经申请的大小(符号位也包括在内，正数有默认的符号位)
    // 0可以被标识为正数，也可以被标识为负数，不受影响
    size_t size = 2;

    // 当前已经使用的位数，便于输出信息(包括符号位)
    size_t used = 2;

    // 第一位空出来，标识正负
    // 之所以选择char，是因为每一位只需要保存一个数字，不需要int型
    bit_type* value;
};
}
