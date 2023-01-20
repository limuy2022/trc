#include <TVM/TRE.hpp>
#include <base/trcdef.hpp>

namespace trc::TVM_space {
/**
 * TVM共享的信息
 */
namespace TVM_share {
    // 由于真假是很常用的值，所以会被提前实例化
    def::INTOBJ true_, false_;
    types::trc_int int_cache[INT_CACHE_LEN];
}

namespace type_int {
    const char* int_name[]
        = { "int", "string", "float", "long int", "long float", "map", "list" };
}
}
