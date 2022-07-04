#pragma once

#include <algorithm>
#include <climits>
#include <cmath>
#include <cstring>
#include <map>
#include <string>
#include <vector>

namespace trc::utils {
/**
 * @brief 由于浮点数有细微的误差，所以需要建立专门的判断函数
 * @tparam floattype double或float
 */
template <typename floattype> bool isequal(floattype a, floattype b) {
    return std::fabs(a - b) < 1e-7;
}

/**
 * @brief 判断是否在start到end的区间中
 * @param start 开始
 * @param end 结尾
 * @param data 数据
 */
template <typename T>
inline bool inrange(const T& start, const T& end, const T& data) {
    return start <= data && data <= end;
}

/**
 * @brief 获取data在v1中的索引值(第一个)
 * @tparam T 容器类型
 * @tparam V 数据类型
 * @param v1 容器
 * @param data 数据
 * @warning C风格字符串不可用
 * @return 索引，未查找到返回-1
 */
template <typename T, typename V> int index_vector(const T& v1, const V& data) {
    auto begin = v1.begin(), end = v1.end();
    auto item = std::find(begin, end, data);
    return item != end ? distance(begin, item) : -1;
}

/**
 * @brief 获取字符串data在v1中的索引值(第一个)
 */
template <typename T> int index_strstl(const T& v1, const char* data) {
    for (int i = 0, n = v1.size(); i < n; i++) {
        if (!strcmp(v1[i], data)) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief 检查data是否在容器T每一项值的里面
 * @tparam T 容器类型
 * @tparam O 数据类型
 * @param data 数据
 * @param arr 容器
 */
template <typename T, typename O> bool check_in(const O& data, T begin, T end) {
    return std::find(begin, end, data) != end;
}

/**
 * @brief 检查data是否在容器T每一项值的里面(字符串专用)
 */
template <typename T> bool str_check_in(const char* data, T begin, T end) {
    for (auto i = begin; i != end; ++i) {
        if (!strcmp(*i, data)) {
            return true;
        }
    }
    return false;
}

/**
 * @brief
 * 检查数据，成功返回元素所在索引，否则返回-1
 * @tparam T 数据类型
 * @tparam O 迭代器类型
 * @param data 数据
 * @param l 容器
 */
template <typename T, typename O>
int check_in_i(const T& data, O begin, O end) {
    int res = 0;
    for (auto i = begin; i != end; ++i) {
        if (*i == data)
            return res;
        res++;
    }
    return -1;
}

/**
 * @brief
 * 检查字符串数据是否在容器中，成功返回元素所在索引，否则返回-1
 * @tparam T 迭代器类型
 * @tparam O 容器类型
 * @param data 数据
 */
template <typename T> int str_check_in_i(const char* data, T begin, T end) {
    int res = 0;
    for (auto i = begin; i != end; ++i) {
        if (!strcmp(*i, data))
            return res;
        res++;
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
bool s_check_in_s(T s1begin, T s1end, O s2begin, O s2end) {
    for (auto i = s1begin; i != s1end; ++i)
        if (!check_in(*i, s2begin, s2end))
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
template <typename T> constexpr size_t sizeof_static_array(T& arr) {
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

/**
 * @brief 求a,b是否满足与数据相匹配
 * @code
 * a=1
 * b=2
 * match_data(a, b, 1, 2)和mathch_data(a,b,2,1)同样返回true
 * @endcode
 */
template <typename T>
inline bool match_data(const T& a, const T& b, const T& c, const T& d) {
    return (a == c && b == d) || (a == d && b == c);
}
}
