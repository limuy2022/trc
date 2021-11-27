#include "base/utils/bytes.h"
#include "base/dll.h"

using namespace std;

namespace trc {
    namespace utils {
        void bytes_order_change(def::byte_t * const &bytes_, int size) {
            /**
             * 改变字节序：大端改成小端，小端改成大端
             * bytes：需要转换的起点
             * size：需要转换的字节数
             * 注：之所以使用char，是因为char正好是一个字节，而且比较好操作，使用之前需要把指针转为（char*）
             */
            int end = size - 1;
            def::byte_t tmp;
            int start = 0;
            for (int i = 0, cnt = size / 2; i < cnt; i++) {
                tmp = bytes_[start + i];
                bytes_[start + i] = bytes_[end - i];
                bytes_[end - i] = tmp;
            }
        }
    }
}
