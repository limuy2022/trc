/**
 * 管理内存申请
 */

#include <new>
#include "base/Error.h"
#include "TVMbase/types/flong.h"
#include "TVMbase/TRE.h"
#include "TVM/TVM.h"
#include "base/memory/memory.h"
#include "base/memory/memory_pool.h"

using namespace std;

namespace trc {
    namespace memory {
        static void nomemory() {
            /**
            * 当没有内存时报错
            */
            error::send_error(error::MemoryError, "can't get the memory from os.");
        }

        /**
         * 管理内存
         */
        memory_pool global_memory_pool;

        void init_mem() {
            /**
             * 内存申请初始化，程序启动时执行
             * 初始化整型和布尔缓存
             */

            set_new_handler(nomemory);
        }
    }
}
