#include <TVM/TRE.h>
#include <base/trcdef.h>

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
    std::string int_name_s[] = { "int", "string", "float", "long int",
        "long float", "map", "array" };
}
}
