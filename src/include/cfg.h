/**
 * 项目配置文件，包括大量基本类型定义
 * 注意：由于大量源文件都引用了本文件，所以尽量不要修改此文件，否则可能会造成编译时间过长
 */ 

#ifndef TRC_INCLUDE_CFG_H
#define TRC_INCLUDE_CFG_H

#include <vector>

using namespace std;

typedef vector<string> vecs;
typedef vector<vector<string> > vecs2d;
typedef vector<char> vecc;

class BigNum;
class trcobj;
class trcint;
class trcfloat;
class trc_string;

typedef trcobj* OBJ;
typedef trcint* INTOBJ;
typedef trcfloat* FLOATOBJ;
typedef trc_string* STRINGOBJ;
typedef BigNum* LONGOBJ;

// 编译时类型标识
#define VAR_TICK 1
#define INT_TICK 2
#define FLOAT_TICK 3
#define STRING_TICK 4
#define LONG_TICK 5
#define FLOAT_L_TICK 6
#define CONST_TICK 7

// 运行时类型标识
#define INT_T 0
#define BOOL_T 1
#define STRING_T 2
#define FLOAT_T 3
#define BIGNUM_T 4
#define BIGFLOAT_T 5

// 当前模块行号
#define LINE_NOW run_env::lines[run_env::run_module]

#endif
