/**
 * 基础数据结构：map
 */

#pragma once

#include "TVMbase/types/base.h"
#include "base/trcdef.h"
#include "TVMbase/dll.h"

using namespace std;

namespace trc {
    namespace TVM_space {
        namespace types {
            namespace map_space {
                class data_info;

                extern const int MAP_MIN_SIZE;
            }
            
            class TRC_TVMbase_api trc_map : public trcobj {
                /**
                 * 用作两个OBJ对象的关联
                 * 具体实现：哈希表加红黑树
                 */
            public:
                trc_map();

                ~trc_map();

                def::OBJ &operator[](const def::OBJ &);

                const def::OBJ &operator[](const def::OBJ &) const;

                void delete_value(const def::OBJ key);

            private:
                // 数据
                map_space::data_info *data_;

                // 哈希表长度
                int length = map_space::MAP_MIN_SIZE;

                // 哈希冲突计数
                int hash_num = 0;

                // 对象个数
                int objs_in = 0;

                int hash_func(void *value) const;

                def::OBJ &get_value(const def::OBJ key) const;

                void resize();
            };
        }
    }
}
