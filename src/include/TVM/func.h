#ifndef TRC_INCLUDE_TVM_FUNC_H
#define TRC_INCLUDE_TVM_FUNC_H

#include <string>
#include <map>
#include "basic_vm.h"

using namespace std;

class frame_:public base
{
public:
    frame_();
};

class func_ {
public:
    // 函数名
    const string name;

    // 字节码信息
    vector<vector<short*> > bytecodes;

    func_(const string &name);
};

#endif
