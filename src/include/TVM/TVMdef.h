/**
 * TVM中的一些的定义
 */

#pragma once

#include <base/trcdef.h>
#include <cstdint>

namespace trc::TVM_space {
class TVM;

class TVM_bytecode;

// 字节码的类型
typedef def::byte_t bytecode_t;
// 字节码索引的类型
typedef uint16_t bytecode_index_t;
// 储存所有字节码的类型
typedef std::vector<TVM_bytecode> struct_codes;
// 指向内置函数的指针
typedef void (*TVM_BUILTINS_FUNC)(int argc, TVM* vm);

// 有参数字节码调用
typedef void (TVM::*ARGV_TVM_METHOD)(bytecode_index_t);

// 与参数字节码调用
typedef void (TVM::*NOARGV_TVM_METHOD)();

// 运行时类型标识
enum class RUN_TYPE_TICK {
    int_T,
    string_T,
    float_T,
    trc_long_T,
    trc_flong_T,
    map_T,
    array_T,
};
}
