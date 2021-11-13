/**
 * 项目配置文件，包括大量基本类型定义
 * 注意：由于大量源文件都引用了本文件，所以尽量不要修改此文件，否则可能会造成编译时间过长
 */

#ifndef TRC_INCLUDE_CFG_H
#define TRC_INCLUDE_CFG_H

#include <vector>
#include <string>

using namespace std;

typedef vector<string> vecs;
typedef vector<vector<string> > vecs2d;
typedef vector<char> vecc;

class trc_long;

class trcobj;

class trc_int;

class trc_float;

class trc_string;

class trc_flong;

typedef trcobj *OBJ;
typedef trc_int *INTOBJ;
typedef trc_float *FLOATOBJ;
typedef trc_string *STRINGOBJ;
typedef trc_long *LONGOBJ;
typedef trc_flong *FLONGOBJ;

// 编译时类型标识
#define VAR_TICK 1
#define int_TICK 2
#define float_TICK 3
#define string_TICK 4
#define LONG_TICK 5
#define FLOAT_L_TICK 6
#define CONST_TICK 7

// 运行时类型标识
#define int_T 0
#define bool_T 1
#define string_T 2
#define float_T 3
#define trc_long_T 4
#define trc_flong_T 5

// 当前模块行号
#define LINE_NOW run_env::lines[run_env::run_module]

#endif
