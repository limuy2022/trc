#ifndef TRC_UTILS_DATA_H
#define TRC_UTILS_DATA_H

#include <string>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

template<typename T, typename V>
int index_vector(vector<T> &v1, const V &data) {
    /**
     * 获取vector的索引值
     */
    auto begin = v1.begin();
    return distance(begin, find(begin, v1.end(), data));
}

template<typename T, typename V>
bool check_in(const T &data, const vector<V> &l) {
    /*
    * 检查数据
    */
    for (const auto &i : l) {
        if (data == i) {
            return true;
        }
    }
    return false;
}

template<typename T>
int check_in_i(const T &data, const vector<T> &l) {
    /**
    * 检查数据
    * 成功返回元素所在索引
    * 否则返回-1
    */

    size_t n = l.size();
    for (int i = 0; i < n; ++i) {
        if (data == l[i]) {
            return i;
        }
    }
    return -1;
}

template<typename T>
bool s_check_in_s(const vector<T> &s1, const vector<T> &s2) {
    /**
    * 检查s1里的元素是否都在s2中
    */
    for (const auto &i : s1)
        if (!check_in(i, s2))
            return false;
    return true;
}

template<typename T>
int os_check_in_s_i(const vector<T> &s1, const vector<T> &s2) {
    /**
    * 检查s1里的元素是否在s2中
    * 成功返回s1中的元素在s2中第一次出现的位置
    * 否则返回-1
    * 注意：检查到一个即返回
    */
    int u;
    for (const auto &i : s1) {
        u = check_in_i(i, s2);
        if (u != -1)
            return u;
    }
    return -1;
}

template<typename T, typename V, typename O>
bool map_check_in_first(const map<T, V> &m1, const O &data) {
    /**
     * 检查data是否在map每一项键的里面
     * O之所以不同，是为了防止const char*和string对比的情况发生
     */
    for (const auto &it : m1)
        if (it.first == data)
            return true;
    return false;
}

template<typename T, typename V, typename O>
bool map_check_in_second(const map<T, V> &m1, const O &data) {
    /**
     * 检查data是否在map每一项值的里面
     */
    for (const auto &it : m1)
        if (it.second == data)
            return true;
    return false;
}

#endif
