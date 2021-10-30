#ifndef TRC_INCLUDE_TVM_TVM_DATA_H
#define TRC_INCLUDE_TVM_TVM_DATA_H

#include <string>
#include <vector>
#include <map>
#include "share.h"
#include "func.h"

using namespace std;

class TVM_data {
	/**
	 * 静态数据
	 * 本类用于分离TVM的职责，装载编译时的数据，如常量池等
	 * 并且降低TVM和Compiler之间的耦合度
	 */ 
public:
	vector<int> const_i;
    vecs const_s, const_name, const_long;
    vector<double> const_f;
    // 注意：此处装载func的静态信息，并不是保存执行信息的场所
    map<string, func_*> funcs;

    float ver_;
    
    vector<vector<short *> > byte_codes;
};

#endif
