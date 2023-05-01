/**
 * 项目配置文件，包括大量基本类型定义
 * 注意：由于大量源文件都引用了本文件，所以尽量不要修改此文件，否则可能会造成编译时间过长
 */

module;
#include <cstdio>
export module trcdef;

namespace trc {
// 行号类型
export typedef size_t line_t;

namespace TVM_space::types {
    class trc_long;
    class trcobj;
    class trc_int;
    class trc_float;
    class trc_string;
    class trc_flong;
}

export namespace def {
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
    const char* version = "0.2";
    // github项目地址
    const char* repo_github = "https://github.com/limuy2022/trc.git";
    // gitee项目地址
    const char* repo_gitee = "https://gitee.com/li-muyangangel/trc.git";
}
}
