#pragma once

#include <sstream>

using namespace std;

static stringstream stream_;

namespace trc {
    namespace utils {
        template<typename outtype, typename intype>
        outtype to_type(const intype &value) {
            /**
             * 任意转化类型，把value转化为outtype类型
             * 但是该函数效率并不算太高
             */

            stream_.clear();
            stream_.str("");
            outtype v;
            stream_ << value;
            stream_ >> v;
            return v;
        }
    }
}
