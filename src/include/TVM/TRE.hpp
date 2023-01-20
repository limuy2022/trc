#pragma once

#include <TVM/TVMdef.hpp>
#include <TVM/types/base.hpp>
#include <TVM/types/trc_int.hpp>
#include <base/Error.hpp>
#include <map>
#include <string>

// int型缓存起始值
#define INT_CACHE_BEGIN (-5)
// int型缓存结束值
#define INT_CACHE_END 256
// int型缓存长度
#define INT_CACHE_LEN 262

namespace trc::TVM_space {
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
    static inline bool obj_to_bool(def::INTOBJ a) {
        return a == true_ ? true : false;
    }
}

namespace type_int {
    extern std::map<std::string, int> name_int;
    extern const char* int_name[];
}
}
