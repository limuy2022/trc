#pragma once

#include <TVM/TVMdef.h>
#include <TVM/library.h>
#include <TVM/types/base.h>
#include <TVM/types/trc_int.h>
#include <base/Error.h>
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
extern def::OBJ firstv, secondv;
extern def::INTOBJ firsti, secondi;
extern def::FLOATOBJ firstf, secondf;
extern def::STRINGOBJ firsts, seconds;
extern def::LONGOBJ firstl, secondl;

namespace TVM_share {
    /**
     * TVM类型数据缓存
     */

    // 布尔值
    extern def::INTOBJ true_, false_;
    // 整型缓存
    extern types::trc_int int_cache[INT_CACHE_LEN];

    /**
     * @brief
     * 将一个代表布尔值的INTOBJ对象转换成布尔值
     */
    inline bool obj_to_bool(def::OBJ a) {
        return a == true_ ? true : false;
    }
}

namespace type_int {
    TRC_TVM_c_api extern std::map<std::string, int> name_int_s;
    TRC_TVM_c_api extern std::string int_name_s[];
}
}
