#include "platform.h"
#include "base/dll.h"

using namespace std;

namespace trc {
    namespace utils {
        bool get_byte_order() {
            /**
             * 获取字节序，true代表大端，false代表小端
             */
            int tmp = 0;
            return !(*(char*)(&tmp));
        }
    }
    
    namespace def {
        // 字节序
        const bool byte_order = utils::get_byte_order();
    }
}
