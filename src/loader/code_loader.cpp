/**
 * 当编译为助记符后转化为对应的整数储存
 */

#include <map>
#include <string>
#include "TVM/TVM.h"

using namespace std;

// 在编译时将助记符转化为字节码
map<string, short> codes_int = {
        {LOAD_INT_, 0},
        {ADD_, 1},
        {NOP_, 2},
        {SUB_, 3},
        {MUL_, 4},
        {DIV_, 5},
        {GOTO_, 6},
        {STORE_NAME_, 7},
        {LOAD_NAME_, 8},
        {DEL_, 9},
        {LOAD_FLOAT_, 10},
        {LOAD_STRING_, 11},
        {CALL_BUILTIN_, 12},
        {IMPORT_, 13},
        {POW_, 14},
        {ZDIV_, 15},
        {MOD_, 16},
        {IF_FALSE_GOTO_, 17},
        {CHANGE_VALUE_, 18},
        {EQUAL_, 19},
        {UNEQUAL_, 20},
        {GREATER_EQUAL_, 21},
        {LESS_EQUAL_, 22},
        {LESS_, 23},
        {GREATER_, 24},
        {ASSERT_, 25},
        {NOT_, 26},
        {AND_, 27},
        {OR_, 28},
        {STORE_LOCAL_, 29},
        {LOAD_LOCAL_, 30},
        {CALL_FUNCTION_, 31},
        {FREE_FUNCTION_, 32},
        {CHANGE_LOCAL_, 33},
        {DEL_LOCAL_, 34},
        {LOAD_LONG_, 35},
        {LOAD_ARRAY_, 36},
        {CALL_METHOD_, 37},
        {LOAD_MAP_, 38}
};

// 在反编译时把字节码转化为助记符
string int_code[] = {
        LOAD_INT_,
        ADD_,
        NOP_,
        SUB_,
        MUL_,
        DIV_,
        GOTO_,
        STORE_NAME_,
        LOAD_NAME_,
        DEL_,
        LOAD_FLOAT_,
        LOAD_STRING_,
        CALL_BUILTIN_,
        IMPORT_,
        POW_,
        ZDIV_,
        MOD_,
        IF_FALSE_GOTO_,
        CHANGE_VALUE_,
        EQUAL_,
        UNEQUAL_,
        GREATER_EQUAL_,
        LESS_EQUAL_,
        LESS_,
        GREATER_,
        ASSERT_,
        NOT_,
        AND_,
        OR_,
        STORE_LOCAL_,
        LOAD_LOCAL_,
        CALL_FUNCTION_,
        FREE_FUNCTION_,
        CHANGE_LOCAL_,
        DEL_LOCAL_,
        LOAD_LONG_,
        LOAD_ARRAY_,
        CALL_METHOD_,
        LOAD_MAP_
};
