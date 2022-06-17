#pragma once

#include <TVM/TVMdef.h>
#include <TVM/library.h>
#include <TVM/types/base.h>
#include <base/trcdef.h>
#include <string>

namespace trc::TVM_space::types {
class TRC_TVM_api trc_string : public trcobj {
public:
    // 字符数(不包括\0)
    size_t char_num = 0;

    char* value;

    trc_string(const trc_string& init);

    trc_string& operator=(const std::string&);

    trc_string(const std::string& init);

    trc_string();

    ~trc_string() override;

    size_t len();

    char& operator[](unsigned int index);

    const char& operator[](unsigned int index) const;

    trc_string& operator=(const trc_string& value_i);

    def::OBJ operator+=(def::OBJ value_i);

    const char* c_str() const;

    void putline(FILE* out) override;

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

    void delete_() override;

private:
    void set_realloc(size_t num);

    // 整型标记类型
    const static RUN_TYPE_TICK type;
};
}
