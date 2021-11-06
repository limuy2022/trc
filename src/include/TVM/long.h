/**
 * 由于c++本身并不支持大整数相加
 * 所以在这里通过trc_long来实现
 * 注意：该类重载了trc所支持的所有运算符
 * 包括+，-，*，/，==，！=等
 */ 

#ifndef TRC_INCLUDE_TRC_LONG_H
#define TRC_INCLUDE_TRC_LONG_H

#include <string>
#include <ostream>
#include "base.h"
#include "cfg.h"

using namespace std;

class trc_long:public trcobj
{
public:
	trc_long& operator=(const string &a);

    trc_long();

    OBJ operator =(OBJ a);

    OBJ operator +(OBJ a);

    OBJ operator -(OBJ a);

    OBJ operator *(OBJ v);

    OBJ operator /(OBJ);

    OBJ operator %(OBJ);

    OBJ pow(OBJ);

    OBJ zdiv(OBJ);

    INTOBJ operator !=(OBJ a);

    INTOBJ operator ==(OBJ);

    INTOBJ operator <(OBJ);

    INTOBJ operator >(OBJ);

    INTOBJ operator <=(OBJ);

    INTOBJ operator >=(OBJ);

    void putline(ostream& out);

    const int& gettype();

    INTOBJ operator!();
    
    INTOBJ operator&&(OBJ value_i);

    INTOBJ operator||(OBJ value_i);

    ~trc_long();

    OBJ to_string();

    OBJ to_float();

    OBJ to_bool();

    void delete_();
    
private:
    void set_alloc(int size_);

    const static int type = trc_long_T;

    // 第一位空出来，标识正负
    //之所以选择char，是因为每一位只需要保存一个数字，不需要int型
    char *value;

    // 当前大整数长度，注意，符号位也包括在内，正数有默认的符号位
    // 0可以被标识为正数，也可以被标识为负数，不受影响
    int size = 1;
};

#endif
