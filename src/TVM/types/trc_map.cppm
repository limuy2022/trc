/**
 * 基础数据结构map
 * 采用哈希表加红黑树实现
 */

module;
#include <cstdlib>
export module trc_map;
import TVMdef;
import base;
import trcdef;
import TVMdef;
import trc_float;
import trc_int;
import trc_string;
import Error;
import language;
import unreach;

// 哈希表大小
#define MAP_MIN_SIZE 50

namespace trc::TVM_space::types {
namespace map_space {
    class data_info;
}

class trc_map : public trcobj {
    /**
     * 用作两个OBJ对象的关联
     * 具体实现：哈希表加红黑树
     */
public:
    trc_map();

    ~trc_map() override;

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

    RUN_TYPE_TICK gettype() override;

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

namespace map_space {
    /**
     * @brief 数据块
     * 哈希表将会申请一块类型为data_info的数组
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
    unreach("map's hash func met a unexpected type");
    return 0;
}

def::OBJ& trc_map::get_value(const def::OBJ key_) const {
    size_t key = hash_func(key_);
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
    size_t key = hash_func(key_);
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
