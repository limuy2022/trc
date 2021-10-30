#ifndef TRC_INCLUDE_TYPECHANGE_H
#define TRC_INCLUDE_TYPECHANGE_H

#include <sstream>

using namespace std;

// 由于stringstream创建销毁的代价过大，所以设在函数之外
static stringstream stream_;

template<typename outtype, typename intype>
outtype to_type(const intype &value) {
    /**
     * 任意转化类型，把value转化为outtype类型
     * 但是该函数效率并不算太高
     */

    stream_.clear();
    stream_.str("");
    outtype v;
    stream_ << value;
    stream_ >> v;
    return v;
}

#endif
