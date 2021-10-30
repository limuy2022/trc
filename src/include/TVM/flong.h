#ifndef TRC_INCLUDE_TVM_TRC_FLONG_H
#define TRC_INCLUDE_TVM_TRC_FLONG_H

#include <string>
#include "memory/mem.h"

using namespace std;

class trc_flong:public trcobj
{
public:
    trc_flong(const string&);

    trc_flong(const double &);

    ~trc_flong();

    void putline(ostream& out);

    int& gettype();
    
private:
    void set_realloc(size_t num);

    // trc_flong在底层是通过int型的动态数组实现的,动态改变大小，
    // 数组大小可能不会刚好对应数位，因为出于效率的考虑，内存会按最多分配，并只有在full_gc启动时才会对多余的空间进行回收
    // 小数点以-1标识
    int *value;
    size_t n;
};

#endif
