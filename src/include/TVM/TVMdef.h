/**
 * TVM中的一些的定义
 */ 

#pragma once

#include "base/trcdef.h"

namespace trc {
    namespace TVM_space {
        class TVM;
        struct TVM_bytecode;

        // 有参数字节码调用
        typedef void (TVM::*ARGV_TVM_METHOD)(const short &);
        // 与参数字节码调用
        typedef void (TVM::*NOARGV_TVM_METHOD)();
        // 字节码的类型
        typedef def::byte_t bytecode_t;
        // 字节码索引的类型
        typedef short index_t;
        // 储存所有字节码的类型
        typedef vector<vector<TVM_bytecode*> > struct_codes;

        // 运行时类型标识
        enum RUN_TYPE_TICK {
            int_T,
            bool_T,
            string_T,
            float_T,
            trc_long_T,
            trc_flong_T,
        };
    }
}
