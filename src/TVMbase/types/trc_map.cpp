﻿/**
 * 基础数据结构map
 * 采用哈希表加红黑树实现
 * 注意：如果你仔细阅读了这里的代码，你会发现这个哈希表其实实现的并不复杂，
 * 效率也只能算是一般，不过有时间再改吧
 */

#include <cstdlib>
#include "TVMbase/types/map.h"
#include "base/Error.h"

using namespace std;

namespace trc {
    namespace TVM_space {
        namespace types {
            namespace map_space {
                class data_info {
                    /**
                     * 数据块，哈希表将会申请一块类型为data_info的数组
                     * 将每一个元素都当成链表来使用
                     */
                public:
                    // object
                    def::OBJ value;
                    data_info *next = nullptr;
                    // 是否有冲突
                    bool flag = false;
                    // 是否有存放数据
                    bool is_data = false;
                    // 未经过哈希函数转换的键
                    void *key;
                };

                // 哈希表大小
                const int MAP_MIN_SIZE = 50;
            }

            using namespace map_space;

            def::OBJ &trc_map::get_value(const def::OBJ key_) const {
                /**
                 * 通过哈希值取到相应的对象地址(具体实现)
                 */
                int key = hash_func(key_);
                data_info &value_ = data_[key];
                // 判断是否存在数据
                if (!value_.is_data) {
                    error::send_error(error::KeyError);
                }
                // 判断是否存在哈希冲突
                if (!value_.flag) {
                    return data_[key].value;
                }
                data_info *back = &value_, *now = back->next;
                while (now != nullptr) {
                    if (now->key == key_) {
                        data_info *tmp = now;
                        now->is_data = false;
                        back->next = now->next;
                        return tmp->value;
                    }
                    back = now->next;
                    now = now->next;
                }
                error::send_error(error::KeyError);
            }

            trc_map::trc_map() :
                    data_((data_info *) malloc(sizeof(data_info) * length)) {
                if (data_ == nullptr) {
                    error::send_error(error::MemoryError, "can't get the memory from os.");
                }
            }

            trc_map::~trc_map() {
                free(data_);
            }

            def::OBJ &trc_map::operator[](const def::OBJ &key) {
                // 调用了get_value函数，只是一层改变对象性质的封装
                return get_value(key);
            }

            const def::OBJ &trc_map::operator[](const def::OBJ &key) const {
                return get_value(key);
            }

            void trc_map::resize() {
                /**
                 * 哈希表扩容，并作判断处理（当哈希冲突超过5次时进行判断）
                 */
                if (objs_in * 1.0 / length >= 0.75) {
                    /* 扩容 */
                    data_ = (data_info *) (realloc(data_, sizeof(data_info) * length * 2));
                    if (data_ == nullptr) {
                        error::send_error(error::MemoryError, "can't get the memory from os.");
                    }
                    // 遍历原先的哈希表长度,做一个记录
                    int tmp = length;
                    // 将哈希表的容量翻倍
                    length *= 2;
                    // 暂时重置冲突
                    hash_num = 0;
                    /* 调用哈希函数进行重复计算 */
                    for (int i = 0; i < tmp; ++i) {
                        if (data_[i].is_data) {
                            data_[i].is_data = false;
                            this->operator[](data_[i].value);
                        }
                    }
                }
            }

            void trc_map::delete_value(const def::OBJ key_) {
                /**
                 * 根据键去删除相应的值
                 */
                int key = hash_func(key_);
                data_info &value_ = data_[key];
                // 键不存在
                if (!value_.is_data) {
                    error::send_error(error::KeyError);
                }
                // 键对上了，判断是否存在链表
                if (value_.key == key_) {
                    value_.is_data = false;
                    if (value_.flag) {
                        data_[key] = *(value_.next);
                    }
                }
                // 存在值但是键不吻合,存在哈希冲突
                data_info *back = &value_, *now = back->next;
                while (now != nullptr) {
                    if (now->key == key_) {
                        data_info *tmp = now;
                        now->is_data = false;
                        back->next = now->next;
                        delete tmp;
                        return;
                    }
                    back = now->next;
                    now = now->next;
                }
                error::send_error(error::KeyError);
            }

            int trc_map::hash_func(void *value) const {
                /**
                 * 哈希函数
                 */
                return *(int*)(&value) % length;
            }
        }
    }
}
