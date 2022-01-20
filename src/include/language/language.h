/**
 * 纯资源的dll，用于储存多种语言
 *
 * 实现原理：将语句存入dll中，通过统一的接口调用不同语言的文字，将每个源文件设置成那种语言的终端编码，所以如果出现编码不同的情况需要自己手动调整编码重新编译
 */

#pragma once

#include "dll.h"

namespace language {
namespace help {
    TRC_language_api extern const char* help_msg;
}
namespace TVM {
    TRC_language_api extern const char*
        type_change_error_msg;
    TRC_language_api extern const char*
        oper_not_def_error_msg;
}
namespace trc {
    TRC_language_api extern const char* mode_not_found;
}
namespace tdb {
    TRC_language_api extern const char* var;
    TRC_language_api extern const char* not_defined;
    TRC_language_api extern const char* start_tip;
    TRC_language_api extern const char* instruction;
}
}
