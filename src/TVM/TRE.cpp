#include "../include/TVM/TRE.h"
#include "../include/cfg.h"

namespace TVM_share {
    /**
     * TVM共享的信息
     */ 

    // 由于真假是很常用的值，所以会被提前实例化
    INTOBJ true_, false_;
    INTOBJ int_cache[INT_CACHE_LEN];
}
