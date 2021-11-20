/**
 * TVM各种数据集合的定义处
 */

#ifndef TRC_INCLUDE_TVM_TVM_DATA_H
#define TRC_INCLUDE_TVM_TVM_DATA_H

#include <string>
#include <vector>
#include <map>
#include "share.h"
#include "func.h"
#include "TVM/TVMdef.h"

using namespace std;

struct TVM_bytecode {
    TVM_bytecode(bytecode_t a, index_t b);
    bytecode_t bycode;
    index_t index;
};

struct TVM_data {
    /**
     * 静态数据
     * 本类用于分离TVM的职责，装载编译时的数据，如常量池等
     * 并且降低TVM和Compiler之间的耦合度
     */
    vector<int> const_i;
    vecs const_s, const_name, const_long;
    vector<double> const_f;
    // 注意：此处装载func的静态信息，并不是保存执行信息的场所
    map<string, func_ *> funcs;

    float ver_;

    struct_codes byte_codes;
};

#endif
