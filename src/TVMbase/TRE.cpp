#include "TVMbase/TRE.h"
#include "base/Error.h"
#include "base/trcdef.h"

namespace trc::TVM_space {
// 中间变量，便于使用
def::OBJ firstv, secondv;

def::INTOBJ firsti, secondi;

def::FLOATOBJ firstf, secondf;

def::STRINGOBJ firsts, seconds;

def::LONGOBJ firstl, secondl;

namespace TVM_share {
    /**
     * TVM共享的信息
     */

    // 由于真假是很常用的值，所以会被提前实例化
    def::INTOBJ true_, false_;
    types::trc_int int_cache[INT_CACHE_LEN];
}

namespace type_int {
    std::string int_name_s[] = { "int", "string", "float",
        "long int", "long float", "map", "array" };
}
}
