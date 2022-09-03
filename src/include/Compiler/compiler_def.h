#pragma once

#include <base/Error.h>

namespace trc::compiler {
typedef error::error_module compiler_error;
/**
 * @brief 编译器的参数
 */
struct compiler_option {
    // 是否生成行号表
    bool number_table;
    // 是否开启优化
    bool optimize;
};

/**
 * @brief 储存编译期间要用到的公共数据和成员
 */
struct compiler_public_data {
    compiler_error error;
    const compiler_option* option;
    // 储存虚拟机常量池的大小
    size_t int_size = 0, float_size = 0, string_size = 0, long_int_size = 0,
           func_size = 0, long_float_size = 0;
};

// 实例化几个常用的编译参数
extern compiler_option optimize_option;
extern compiler_option nooptimize_option;
}
