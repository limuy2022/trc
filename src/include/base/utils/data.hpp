#pragma once

#include <string>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

namespace trc {
    namespace utils {
        template<typename T, typename V>
        int index_vector(const vector<T> &v1, const V &data) {
            /**
             * 获取vector的索引值
             * v1:容器
             */
            const auto &begin = v1.begin();
            return distance(begin, find(begin, v1.end(), data));
        }

        template<typename T, typename O>
        bool check_in(const O &data, const T& arr) {
            /**
             * 检查data是否在容器(包括数组)每一项值的里面
             * data:数据
             * arr:容器
             * size_:容器长度
             */
            const auto &end = arr.end();
            return find(arr.begin(), end, data) != end;
        }

        template<typename T, typename O>
        int check_in_i(const T &data, const O &l) {
            /**
            * 检查数据，成功返回元素所在索引，否则返回-1
            * data:数据
            * l:容器
            * n:容器长度
            */

            size_t n = l.size();
            for (size_t i = 0; i < n; ++i)
                if (data == l[i])
                    return i;
            return -1;
        }

        template<typename T, typename O>
        bool s_check_in_s(const T &s1, const O &s2) {
            /**
            * 检查s1里的元素是否都在s2中
            * l1:容器s1长度
            * l2:容器s2长度
            */

            const auto& end = s1.end();
            for (auto i = s1.begin(); i != end; ++i)
                if (!check_in(*i, s2))
                    return false;
            return true;
        }

        template<typename T, typename V>
        int os_check_in_s_i(const T &s1, const V &s2) {
            /**
            * 检查s1里的元素是否在s2中, 成功返回s1中的元素在s2中第一次出现的位置, 否则返回-1
            * l1:容器s1长度
            * l2:容器s2长度
            * 注意：检查到一个即返回
            */
            int u;
            const auto &end = s1.end();
            for (auto i = s1.begin(); i != end; ++i) {
                if ((u = check_in_i(*i, s2)) != -1)
                    return u;
            }
            return -1;
        }

        template<typename T, typename V, typename O>
        bool map_check_in_first(const map<T, V> &m1, const O &data) {
            /**
             * 检查data是否在map每一项键的里面
             */
            for (const auto &it: m1)
                if (it.first == data)
                    return true;
            return false;
        }

        template<typename T, typename V, typename O>
        bool map_check_in_second(const map<T, V> &m1, const O &data) {
            /**
             * 检查data是否在map每一项值的里面
             */
            for (const auto &it: m1)
                if (it.second == data)
                    return true;
            return false;
        }

        template<typename T>
        constexpr size_t get_index_static_array(T &arr) {
            return sizeof(arr) / sizeof(arr[0]);
        }
    }
}
