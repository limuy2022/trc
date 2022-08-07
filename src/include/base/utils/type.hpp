#pragma once

#include <sstream>

static std::stringstream stream_;

namespace trc::utils {
/**
 * @brief 任意转化类型
 * @param value 需要转换的数据
 * @returns 转换后的值
 * @warning 该函数效率并不算太高，请优先求助标准库例如atoi等
 * @tparam outtype 输出类型(需要自己指定)
 * @tparam intype 输入类型(无需自己指定)
 * @code to_type<int>("10") -> 10 @endcode
 */
template <typename outtype, typename intype>
outtype to_type(const intype& value) {
    stream_.clear();
    stream_.str("");
    outtype v;
    stream_ << value;
    stream_ >> v;
    return v;
}
}
