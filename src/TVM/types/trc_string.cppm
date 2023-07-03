/**
 * TVM中的字符串类型
 * 注：自己实现的原因：
 * 1.本次实现的string不需要有太多的功能，而标准库中string内容太多，功能太繁杂
 * 2.作为一个类型，需要被gc统一管理，所以自己实现
 *
 * 本次实现的string用法和标准库的差别很大，本次string只为TVM实现
 * 另外，trc_string类并不提供对char和char*进行操作的函数，基本仅支持trc_string本身
 */

module;
#include <cstdio>
#include <cstring>
#include <string>
export module trc_string;
import TVMdef;
import base;
import trcdef;

export namespace trc::TVM_space::types {
class trc_string : public trcobj {
public:
    // 字符数(不包括\0的长度)
    size_t char_num = 0;

    char* value;

    trc_string(const trc_string& init);

    trc_string& operator=(const std::string&);

    explicit trc_string(const std::string& init);

    trc_string();

    ~trc_string() override;

    [[nodiscard]] size_t len() const;

    char& operator[](unsigned int index) const;

    trc_string& operator=(const trc_string& value_i);

    def::OBJ operator+=(def::OBJ value_i);

    [[nodiscard]] const char* c_str() const;

    void putline(FILE* out) override;

    /**
     * @brief 读入数据
     * @param in_ 文件流
     * @warning 输入与标准库的方式不同，并不是读到空格停止，而是读到换行符停止
     */
    void in(FILE* in_);

    def::OBJ to_int() override;

    def::OBJ to_float() override;

    def::INTOBJ operator==(def::OBJ value_i) override;

    def::INTOBJ operator!=(def::OBJ value_i) override;

    def::INTOBJ operator<(def::OBJ value_i) override;

    def::INTOBJ operator>(def::OBJ value_i) override;

    def::INTOBJ operator<=(def::OBJ value_i) override;

    def::INTOBJ operator>=(def::OBJ value_i) override;

    def::OBJ operator+(def::OBJ value_i) override;

    def::OBJ operator*(def::OBJ value_i) override;

    RUN_TYPE_TICK gettype() override;

private:
    /**
     * @brief 重新申请字符数
     * @warning num中不包括\0
     */
    void set_realloc(size_t num);

    // 整型标记类型
    const static RUN_TYPE_TICK type;
};
}
