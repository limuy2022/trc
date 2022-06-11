#pragma once

namespace trc::memory {
/**
 * @brief 通过交换强制释放所有内存
 * @warning
 * 由于效率极低，应当谨慎使用，只有当需要释放stack等stl的内存使才使用
 * @tparam T 容器类型
 * @param f 容器
 */
template <typename T> inline void free_stl(T& f) {
    T().swap(f);
}

/**
 * @brief 释放容器内所有动态数组的内存
 * @tparam V 容器类型
 * @param f 容器
 * @warning 仅限于通过new[]申请的内存
 */
template <typename V> void free_array_array(V& f) {
    for (auto i : f) {
        delete[] i;
    }
    f.clear();
}

/**
 * @brief 释放整个容器中元素的内存
 * @tparam T 容器类型
 * @param f 容器
 * @warning 仅限于通过new申请的内存
 */
template <typename T> void free_array_obj(T& f) {
    for (auto i : f) {
        delete i;
    }
    f.clear();
}
}
