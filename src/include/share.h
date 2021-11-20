#ifndef TRC_INCLUDE_SHARE_H
#define TRC_INCLUDE_SHARE_H

#include <map>
#include "trcdef.h"

using namespace std;

extern const float version;
extern const vecc num;

namespace type_int {
    extern map<string, int> name_int_s;
    extern string int_name_s[];
}

namespace run_env {
    // 执行环境

    extern string run_module;
    // 当前执行到的模块函数
    extern map<string, int> lines;

    void set_module(const std::string &name);
}

#endif
