#ifndef TRC_TVM_STRING_H
#define TRC_TVM_STRING_H

#include <string>
#include "base.h"
#include "../../include/cfg.h"

using namespace std;

class trc_string : public trcobj
{
public:
    char *value;

    // 字符数
    size_t char_num;

    trc_string(const trc_string &init);

    trc_string& operator=(const string&);

    trc_string(const string &init);

    trc_string();

    ~trc_string();

    size_t len();

    char& operator[](unsigned int index);

    const char& operator[](unsigned int index) const;

    OBJ operator=(const trc_string& value_i);

    OBJ operator+=(trcobj *value_i);

    const char * c_str();

    void putline(ostream& out);

    friend
    istream& operator>>(istream &in_, trc_string &data_);

    INTOBJ to_int();

    FLOATOBJ to_float();

    INTOBJ operator==(OBJ value_i);

    INTOBJ operator!=(OBJ value_i);
    
    INTOBJ operator<(OBJ value_i);
    
    INTOBJ operator>(OBJ value_i);
    
    INTOBJ operator<=(OBJ value_i);
    
    INTOBJ operator>=(OBJ value_i);
    
    OBJ operator+(OBJ value_i);
    
    OBJ operator*(OBJ value_i);
    
    int& gettype();

    void delete_();

private:
    void set_realloc(size_t num);

    // 整型标记类型
    int type = STRING_T;
};

#endif
