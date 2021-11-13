#ifndef TRC_INCLUDE_TVM_FLOAT_H
#define TRC_INCLUDE_TVM_FLOAT_H

#include "base.h"
#include "cfg.h"

using namespace std;

class trc_float : public trcobj {
public:
    double value;

    void putline(ostream &out);

    ~trc_float();

    trc_float();

    trc_float(const double &value);

    OBJ to_int();

    OBJ to_string();

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
    const static int type = float_T;
};

#endif
