#ifndef TRC_INCLUDE_TVM_INT_H
#define TRC_INCLUDE_TVM_INT_H

#include "base.h"
#include "cfg.h"

using namespace std;

class trc_int : public trcobj {
public:
    int value = 0;

    void putline(ostream &out);

    ~trc_int();

    trc_int();

    trc_int(int value);

    OBJ to_string();

    OBJ to_float();

    OBJ to_bool();

    INTOBJ operator==(OBJ value_i);

    INTOBJ operator!=(OBJ value_i);

    INTOBJ operator<(OBJ value_i);

    INTOBJ operator>(OBJ value_i);

    INTOBJ operator<=(OBJ value_i);

    INTOBJ operator>=(OBJ value_i);

    OBJ operator+(OBJ value_i);

    OBJ operator-(OBJ value_i);

    OBJ operator*(OBJ value_i);

    OBJ operator/(OBJ value_i);

    OBJ operator%(OBJ value_i);

    OBJ pow_(OBJ value_i);

    OBJ zdiv(OBJ value_i);

    INTOBJ operator!();

    INTOBJ operator&&(OBJ value_i);

    INTOBJ operator||(OBJ value_i);

    const int &gettype();

private:
    // 整型标记类型
    const static int type = int_T;
};

#endif
