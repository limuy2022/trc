#pragma once

#include <algorithm>
#include <map>
#include <string>
#include <vector>

namespace trc::utils {
/**
 * @brief 判断是否在start到end的区间中
 * @param start 开始
 * @param end 结尾
 * @param data 数据
 */
template <typename T>
inline bool inrange(
    const T& start, const T& end, const T& data) {
    return start <= data && data <= end;
}

/**
 * @brief 获取data在v1中的索引值
 * @tparam T 容器类型
 * @tparam V 数据类型
 * @param v1 容器
 * @param data 数据
 * @return 索引，未查找到返回-1
 */
template <typename T, typename V>
int index_vector(const T& v1, const V& data) {
    auto begin = v1.begin(), end = v1.end();
    auto item = std::find(begin, end, data);
    return item != end ? distance(begin, item) : -1;
}

/**
 * @brief 检查data是否在容器T每一项值的里面
 * @tparam T 容器类型
 * @tparam O 数据类型
 * @param data 数据
 * @param arr 容器
 */
template <typename T, typename O>
bool check_in(const O& data, const T& arr) {
    auto end = arr.end();
    return std::find(arr.begin(), end, data) != end;
}

/**
 * @brief
 * 检查数据，成功返回元素所在索引，否则返回-1
 * @tparam T 数据类型
 * @tparam O 容器类型
 * @param data 数据
 * @param l 容器
 */
template <typename T, typename O>
int check_in_i(const T& data, const O& l) {
    size_t index = 0;
    for (const auto& i : l) {
        if (data == i)
            return index;
        index++;
    }
    return -1;
}

/**
 * @brief 检查s1里的元素是否都在s2中
 * @tparam T 容器1类型
 * @tparam O 容器2类型
 * @param s1 容器1
 * @param s2 容器2
 */
template <typename T, typename O>
bool s_check_in_s(const T& s1, const O& s2) {
    auto end = s1.end();
    for (auto i = s1.begin(); i != end; ++i)
        if (!check_in(*i, s2))
            return false;
    return true;
}

/**
 * @brief 检查data是否是map的键
 * @tparam T map的类型(map,unordered_map)
 * @tparam O 数据的类型
 * @param m1 map
 * @param data 数据
 */
template <typename T, typename O>
bool map_check_in_first(const T& m1, const O& data) {
    for (const auto& it : m1)
        if (it.first == data)
            return true;
    return false;
}

/**
 * @brief 检查data是否是map的值
 * @tparam T map的类型（map,unordered_map）
 * @tparam O 数据类型
 * @param m1 map
 * @param data 数据
 */
template <typename T, typename O>
bool map_check_in_second(const T& m1, const O& data) {
    for (const auto& it : m1)
        if (it.second == data)
            return true;
    return false;
}

/**
 * @brief 获取数组的长度
 * @tparam T 数组类型
 * @param arr 数组
 * @return 数组的长度
 */
template <typename T>
constexpr size_t sizeof_static_array(T& arr) {
    return sizeof(arr) / sizeof(arr[0]);
}

/**
 * @brief 求整型长度
 * @param in 数据
 * @details 具体优化方法：按多位计算，减少除法数量
 */
template <typename T> unsigned int len(T in) {
    // 负数就提升为正数
    if (in < 0) {
        in = -in;
    }
    int out = 0;
    do {
        if (in < 10) {
            return out + 1;
        } else if (in < 100) {
            return out + 2;
        } else if (in < 1000) {
            return out + 3;
        } else if (in < 10000) {
            return out + 4;
        } else if (in < 100000) {
            return out + 5;
        }
        in /= 100000;
        out += 5;
    } while (in != 0);
    return out;
}
}
