/**
 * 基础数据结构：map
 */

#ifndef TRC_INCLUDE_TVM_MAP_H
#define TRC_INCLUDE_TVM_MAP_H

#include "base.h"
#include "trcdef.h"

using namespace std;

namespace map_space {
    class data_info;

    extern const int MAP_MIN_SIZE;
}

class trc_map : public trcobj {
    /**
     * 用作两个OBJ对象的关联
     * 具体实现：哈希表加红黑树
     */
public:
    trc_map();

    ~trc_map();

    OBJ &operator[](const OBJ &);

    const OBJ &operator[](const OBJ &) const;

    void delete_value(const OBJ key);

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

    OBJ &get_value(const OBJ key) const;

    void resize();
};

#endif
