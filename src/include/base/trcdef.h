/**
 * 项目配置文件，包括大量基本类型定义
 * 注意：由于大量源文件都引用了本文件，所以尽量不要修改此文件，否则可能会造成编译时间过长
 */

#pragma once

#include <array>
#include <cstdio>
#include <string>
#include <vector>

typedef std::vector<std::string> vecs;

/**
 * @param ... 报错的辅助输出信息
 */
#define NOREACH(...)                                                           \
    do {                                                                       \
        fprintf(stderr, __VA_ARGS__);                                          \
        trc::noreach_internal(__LINE__, __FUNCTION__, __FILE__);               \
    } while (0)

namespace trc {
/**
 * @brief
 * 在不能被执行到的地方放上这条语句，出现问题会强行停止程序
 */
void noreach_internal(size_t line, const char* funcname, const char* filename);

namespace TVM_space::types {
    class trc_long;

    class trcobj;

    class trc_int;

    class trc_float;

    class trc_string;

    class trc_flong;
}
namespace def {
    // 虚拟机对象
    typedef TVM_space::types::trcobj* OBJ;
    typedef TVM_space::types::trc_int* INTOBJ;
    typedef TVM_space::types::trc_float* FLOATOBJ;
    typedef TVM_space::types::trc_string* STRINGOBJ;
    typedef TVM_space::types::trc_long* LONGOBJ;
    typedef TVM_space::types::trc_flong* FLONGOBJ;

    // 字节类型
    typedef unsigned char byte_t;
    // 版本号
    extern const char* version;
    // github项目地址
    extern const char* repo_github;
    // gitee项目地址
    extern const char* repo_gitee;
}
}
