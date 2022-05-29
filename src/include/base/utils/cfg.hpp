/**
 * 读写配置文件
 */

#pragma once

#include "base/Error.h"
#include "language/error.h"
#include <fstream>
#include <map>
#include <string>

namespace trc {
namespace utils {
    /**
     * @brief 管理配置文件的类
     * @tparam t1 键的类型
     * @tparam t2 值的类型
     */
    template <typename t1, typename t2> class cfg {
    public:
        /**
         * @param path 配置文件路径
         */
        cfg(const std::string& path);

        /**
         * @brief 重新读取配置文件
         * @param path 文件路径
         */
        void reload(const std::string& path);

        t2& operator[](const t1&);

        const t2& operator[](const t1&) const;

    private:
        std::map<t1, t2> data;

        /**
         * @brief 加载文件
         * 构造函数和reload函数都调用了它
         */
        void load_file(const std::string& path);
    };

    template <typename t1, typename t2>
    void cfg<t1, t2>::load_file(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            error::send_error(error::OpenFileError,
                language::error::openfileerror,
                path.c_str());
        }
        t1 tmp1;
        t2 tmp2;
        file >> tmp1 >> tmp2;
        data[tmp1] = tmp2;
        file.close();
    }

    template <typename t1, typename t2>
    cfg<t1, t2>::cfg(const std::string& path) {
        load_file(path);
    }

    template <typename t1, typename t2>
    void cfg<t1, t2>::reload(const std::string& path) {
        load_file(path);
    }

    template <typename t1, typename t2>
    t2& cfg<t1, t2>::operator[](const t1& key) {
        return data[key];
    }

    template <typename t1, typename t2>
    const t2& cfg<t1, t2>::operator[](const t1& key) const {
        return data[key];
    }
}
}
