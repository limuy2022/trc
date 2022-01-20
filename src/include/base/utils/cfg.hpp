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
    /*t1是键的类型，t2是值的类型*/
    template <typename t1, typename t2> class cfg {
        /**
         * 管理配置文件的类
         */
    public:
        // path:配置文件路径
        cfg(const std::string& path);

        // 重新读取配置文件
        void reload(const std::string& path);

        // 根据键获取值
        t2& operator[](const t1&);

        const t2& operator[](const t1&) const;

    private:
        std::map<t1, t2> data;

        void load_file(const std::string& path);
    };

    template <typename t1, typename t2>
    void cfg<t1, t2>::load_file(const std::string& path) {
        /**
         * 加载文件
         * 构造函数和reload函数都调用了它
         */
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
