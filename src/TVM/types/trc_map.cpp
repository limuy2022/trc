/**
 * 基础数据结构map
 * 采用哈希表加红黑树实现
 * 注意：如果你仔细阅读了这里的代码，你会发现这个哈希表其实实现的并不复杂，
 * 效率也只能算是一般，不过有时间再改吧
 */

#include <TVM/TVMdef.h>
#include <TVM/types/trc_float.h>
#include <TVM/types/trc_int.h>
#include <TVM/types/trc_map.h>
#include <TVM/types/trc_string.h>
#include <base/Error.h>
#include <cstdlib>
#include <language/error.h>

namespace trc::TVM_space::types {
namespace map_space {
    /**
     * 数据块，哈希表将会申请一块类型为data_info的数组
     * 将每一个元素都当成链表来使用
     */
    class data_info {
    public:
        // object
        def::OBJ value;
        data_info* next = nullptr;
        // 是否有冲突,false代表没有，true代表有
        bool flag = false;
        // 是否有存放数据
        bool is_data = false;
        // 未经过哈希函数转换的键
        void* key;
    };
}

// 因为返回值是引用，所以需要对nullptr进行类型包装
def::OBJ error_tick = nullptr;

using namespace map_space;

const RUN_TYPE_TICK trc_map::type = RUN_TYPE_TICK::map_T;

RUN_TYPE_TICK trc_map::gettype() {
    return type;
}

size_t trc_map::hash_func(def::OBJ value) const {
    RUN_TYPE_TICK tag = value->gettype();
    if (tag == RUN_TYPE_TICK::int_T) {
        return ((def::INTOBJ)value)->value % length;
    } else if (tag == RUN_TYPE_TICK::string_T) {
        char* tmp = ((def::STRINGOBJ)value)->value;
        size_t ans = 0;
        while (*tmp) {
            ans += *tmp;
            ans %= length;
            tmp++;
        }
        return ans;
    } else if (tag == RUN_TYPE_TICK::float_T) {
        return int(((def::FLOATOBJ)value)->value) % length;
    }
}

def::OBJ& trc_map::get_value(const def::OBJ key_) const {
    int key = hash_func(key_);
    data_info& value_ = data_[key];
    // 判断是否存在数据
    if (!value_.is_data) {
        return error_tick;
    }
    // 判断是否存在哈希冲突
    if (!value_.flag) {
        return data_[key].value;
    }
    data_info *back = &value_, *now = back->next;
    while (now != nullptr) {
        if (now->key == key_) {
            data_info* tmp = now;
            now->is_data = false;
            back->next = now->next;
            return tmp->value;
        }
        back = now->next;
        now = now->next;
    }
    return error_tick;
}

trc_map::trc_map()
    : data_((data_info*)malloc(sizeof(data_info) * length)) {
    if (data_ == nullptr) {
        error::send_error(error::MemoryError, language::error::memoryerror);
    }
}

trc_map::~trc_map() {
    free(data_);
}

def::OBJ& trc_map::operator[](const def::OBJ& key) {
    return get_value(key);
}

const def::OBJ& trc_map::operator[](const def::OBJ& key) const {
    return get_value(key);
}

void trc_map::resize() {
    if (objs_in * 1.0 / length >= 0.75) {
        /* 扩容 */
        data_ = (data_info*)(realloc(data_, sizeof(data_info) * length * 2));
        if (data_ == nullptr) {
            error::send_error(error::MemoryError, language::error::memoryerror);
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

bool trc_map::delete_value(const def::OBJ key_) {
    int key = hash_func(key_);
    data_info& value_ = data_[key];
    // 键不存在
    if (!value_.is_data) {
        return false;
    }
    // 键对上了，判断是否存在链表
    if (value_.key == key_) {
        value_.is_data = false;
        if (value_.flag) {
            // 如果后续有值，将它往前挪一格
            data_[key] = *(value_.next);
        }
    }
    // 存在值但是键不吻合,存在哈希冲突
    data_info *back = &value_, *now = back->next;
    while (now != nullptr) {
        if (now->key == key_) {
            data_info* tmp = now;
            now->is_data = false;
            back->next = now->next;
            delete tmp;
            return true;
        }
        back = now->next;
        now = now->next;
    }
    return false;
}
}
