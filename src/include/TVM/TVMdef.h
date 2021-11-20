#ifndef TRC_INCLUDE_TVM_TVMDEF_H
#define TRC_INCLUDE_TVM_TVMDEF_H

#include "trcdef.h"

// 字节码的类型
typedef byte_t bytecode_t;
// 字节码索引的类型
typedef short index_t;
// 储存所有字节码的类型
typedef vector<vector<TVM_bytecode*> > struct_codes;

#endif
