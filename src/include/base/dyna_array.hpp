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
    delete[] array;
}

template <typename T> void dyna_array<T>::compress_memory() {
    T* new_array = new T[size];
    for (size_t i = 0; i < size; ++i) {
        new_array[i] = std::move(array[i]);
    }
    delete[] array;
    array = new_array;
}

template <typename T> T* dyna_array<T>::end() {
    return array + size;
}

template <typename T> void dyna_array<T>::clear() {
    size = 0;
    delete[] array;
    array = nullptr;
}

template <typename T> T& dyna_array<T>::operator[](size_t index) {
    return array[index];
}
}