/**
 * 基础数据结构：map
 */

#pragma once

#include <TVM/TVMdef.h>
#include <TVMbase/library.h>
#include <TVMbase/types/base.h>
#include <base/trcdef.h>

// 哈希表大小
#define MAP_MIN_SIZE 50

namespace trc::TVM_space::types {
namespace map_space {
    class data_info;
}

class TRC_TVMbase_api trc_map : public trcobj {
    /**
     * 用作两个OBJ对象的关联
     * 具体实现：哈希表加红黑树
     */
public:
    trc_map();

    ~trc_map();

    /**
     * @brief 根据键取值
     * @details
     * 调用了get_value函数，只是一层改变对象性质的封装
     */
    def::OBJ& operator[](const def::OBJ&);

    /**
     * @brief 根据键取值
     * @details
     * 调用了get_value函数，只是一层改变对象性质的封装
     */
    const def::OBJ& operator[](const def::OBJ&) const;

    /**
     * @brief 根据键去删除相应的值
     * @return true代表正常
     */
    bool delete_value(const def::OBJ key);

    RUN_TYPE_TICK gettype();

private:
    // 哈希表长度
    int length = MAP_MIN_SIZE;

    // 数据
    map_space::data_info* data_;

    const static RUN_TYPE_TICK type;

    // 哈希冲突计数
    int hash_num = 0;

    // 对象个数
    int objs_in = 0;

    /**
     * @brief 哈希函数，计算出哈希值
     */
    size_t hash_func(def::OBJ value) const;

    /**
     * @brief
     * 通过哈希值取到相应的对象地址(具体实现)
     */
    def::OBJ& get_value(const def::OBJ key) const;

    /**
     * @brief 哈希表扩容
     * @details 当哈希冲突超过5次时进行判断
     */
    void resize();
};
}
