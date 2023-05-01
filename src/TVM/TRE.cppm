module;
#include <map>
#include <string>
export module TRE;
import TVMdef;
import base;
import trc_int;
import Error;
import trcdef;

// int型缓存起始值
export constexpr int INT_CACHE_BEGIN = -5;
// int型缓存结束值
export constexpr int INT_CACHE_END = 256;
// int型缓存长度
export constexpr int INT_CACHE_LEN = 262;

export namespace trc::TVM_space {
/**
 * @brief TVM类型数据缓存和TVM共享的信息
 */
namespace TVM_share {
    // 布尔值
    def::INTOBJ true_, false_;
    // 整型缓存
    types::trc_int int_cache[INT_CACHE_LEN];

    const char* int_name[] = { "int", "string", "float", "long int",
            "long float", "map", "list" };

    /**
     * @brief
     * 将一个代表布尔值的INTOBJ对象转换成布尔值
     */
    inline bool obj_to_bool(def::INTOBJ a) {
        return a == true_ ? true : false;
    }
}
}
