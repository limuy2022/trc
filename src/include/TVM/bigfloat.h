#ifndef TRC_INCLUDE_TVM_BIGFLOAT_H
#define TRC_INCLUDE_TVM_BIGFLOAT_H

#include <string>
#include "../memory/mem.h"

using namespace std;

class BigFloat:public trcobj
{
public:
    BigFloat(const string&);

    BigFloat(const double &);

    ~BigFloat();

    void putline(ostream& out);

    int& gettype();
    
private:
    void set_realloc(size_t num);

    // BigFloat在底层是通过int型的动态数组实现的,动态改变大小，
    // 数组大小可能不会刚好对应数位，因为出于效率的考虑，内存会按最多分配，并只有在full_gc启动时才会对多余的空间进行回收
    // 小数点以-1标识
    int *value;
    size_t n;
};

#endif
