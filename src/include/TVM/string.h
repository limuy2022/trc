#ifndef TRC_INCLUDE_TVM_STRING_H
#define TRC_INCLUDE_TVM_STRING_H

#include <string>
#include "base.h"
#include "cfg.h"

using namespace std;

class trc_string : public trcobj {
public:
    char *value;

    // 字符数(不包括\0)
    size_t char_num = 0;

    trc_string(const trc_string &init);

    trc_string &operator=(const string &);

    trc_string(const string &init);

    trc_string();

    ~trc_string();

    size_t len();

    char &operator[](unsigned int index);

    const char &operator[](unsigned int index) const;

    OBJ operator=(const trc_string &value_i);

    OBJ operator+=(trcobj *value_i);

    const char *c_str();

    void putline(ostream &out);

    friend
    istream &operator>>(istream &in_, trc_string &data_);

    OBJ to_int();

    OBJ to_float();

    INTOBJ operator==(OBJ value_i);

    INTOBJ operator!=(OBJ value_i);

    INTOBJ operator<(OBJ value_i);

    INTOBJ operator>(OBJ value_i);

    INTOBJ operator<=(OBJ value_i);

    INTOBJ operator>=(OBJ value_i);

    OBJ operator+(OBJ value_i);

    OBJ operator*(OBJ value_i);

    const int &gettype();

    void delete_();

private:
    void set_realloc(size_t num);

    // 整型标记类型
    const static int type = string_T;
};

#endif
