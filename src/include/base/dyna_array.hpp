#pragma once

#include <cstdlib>

namespace trc::utils {
/**
 * 简单的动态数组封装
 * @tparam T 必须为POD类型
 * @warning 请使用malloc申请
 */
template <typename T> class dyna_array {
public:
    ~dyna_array();

    /**
     * 用于压缩使用的内存，调整内存用多少分配多少
     */
    void compress_memory();

    T* array = nullptr;

    size_t size = 0;

    T* end();

    void clear();

    T& operator[](size_t index);
};

template <typename T> dyna_array<T>::~dyna_array() {
    free(array);
}

template <typename T> void dyna_array<T>::compress_memory() {
    // 因为该函数用于压缩内存，所以不存在分配失败的可能，故不做null校验
    array = (T*)realloc(array, sizeof(T) * size);
}

template <typename T> T* dyna_array<T>::end() {
    return array + size;
}

template <typename T> void dyna_array<T>::clear() {
    size = 0;
    free(array);
    array = nullptr;
}

template <typename T> T& dyna_array<T>::operator[](size_t index) {
    return array[index];
}
}