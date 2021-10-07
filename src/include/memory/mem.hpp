#ifndef TRC_INCLUDE_MEMORY_MEM_HPP
#define TRC_INCLUDE_MEMORY_MEM_HPP

template<typename T>
void free_stl(T &f) {
    /**
     * 通过交换强制释放所有内存
     * 注：由于效率极低，应当谨慎使用，只有当需要释放stack等stl的内存使才使用
     */

    T().swap(f);
}

template<typename V>
void free_vector(V &f) {
    /*
    * 释放容器内所有动态数组的内存，包括但不限于vector
    */

    for (const auto &i : f) {
        delete []i;
    }
    f.clear();
}

#endif
