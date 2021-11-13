#ifndef TRC_INCLUDE_TVM_TRE_H
#define TRC_INCLUDE_TVM_TRE_H

#include <string>
#include "base.h"
#include "memory/mem.h"

// int型缓存起始值
#define INT_CACHE_BEGIN -5
// int型缓存结束值
#define INT_CACHE_END 256
// int型缓存长度
#define INT_CACHE_LEN 262

// 有参数字节码调用
typedef void (TVM::*ARGV_TVM_METHOD)(const short &);

// 与参数字节码调用
typedef void (TVM::*NOARGV_TVM_METHOD)();

using namespace std;

class TVM;

class gc_control;

namespace TRE {
    /**
     * Trc运行时环境
     */
    extern NOARGV_TVM_METHOD TVM_RUN_CODE_NOARG_FUNC[];
    extern ARGV_TVM_METHOD TVM_RUN_CODE_ARG_FUNC[];
}

namespace TVM_share {
    /**
     * TVM类型数据缓存
     */

    // 布尔值
    extern INTOBJ true_, false_;
    // 整型缓存
    extern INTOBJ int_cache[INT_CACHE_LEN];
}

#endif
