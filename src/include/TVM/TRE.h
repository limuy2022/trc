﻿#pragma once

#include "TVMdef.h"
#include "base/Error.h"
#include "library.h"
#include <TVM/types/base.h>
#include <TVM/types/int.h>
#include <map>
#include <string>

// int型缓存起始值
#define INT_CACHE_BEGIN -5
// int型缓存结束值
#define INT_CACHE_END 256
// int型缓存长度
#define INT_CACHE_LEN 262

namespace trc::TVM_space {
// 中间变量，便于使用
TRC_TVM_api extern def::OBJ firstv, secondv;

TRC_TVM_api extern def::INTOBJ firsti, secondi;

TRC_TVM_api extern def::FLOATOBJ firstf, secondf;

TRC_TVM_api extern def::STRINGOBJ firsts, seconds;

TRC_TVM_api extern def::LONGOBJ firstl, secondl;

namespace TVM_share {
    /**
     * TVM类型数据缓存
     */

    // 布尔值
    TRC_TVM_api extern def::INTOBJ true_, false_;
    // 整型缓存
    TRC_TVM_api extern types::trc_int int_cache[INT_CACHE_LEN];

    /**
     * @brief
     * 将一个代表布尔值的INTOBJ对象转换成布尔值
     */
    inline bool obj_to_bool(def::OBJ a) {
        return a == true_ ? true : false;
    }
}

namespace type_int {
    TRC_TVM_api extern std::map<std::string, int> name_int_s;
    TRC_TVM_api extern std::string int_name_s[];
}
}
