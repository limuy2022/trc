/**
 * @file io.hpp
 * @brief 输入输出
 * @date 2022-05-13
 *
 * @copyright Copyright (c) 2022
 *
 */

module;
#include <cstdio>
#include <cstdlib>
#include <sstream>
export module io;
import error;

const size_t mem_init_size = 15;
const size_t mem_realloc_size = 20;

export namespace trc::io {
/**
 * @brief 读入一个长度不限的字符串(不包括换行符)
 * @details 具体使用可以参考getline(cin, str)函数的使用
 * @warning
 * 参数必须是未分配内存的，已分配的需要在进函数之前释放掉
 * @return true:正确读取,没有读取到尾部，false:读取完毕，已经读取到文件流尾部
 */
bool readstr(char*& str, FILE* stream) {
    size_t len = mem_init_size;
    str = (char*)malloc((len + 1) * sizeof(char));
    if (str == nullptr) {
        error::send_error(error::MemoryError, language::error::memoryerror);
    }
    size_t index = 0;
    // 采用int是为了检测出EOF标志
    int c = fgetc(stream);
    while (c != '\n') {
        if (c == EOF) {
            // 文件读取完毕
            str[index] = '\0';
            return false;
        }
        if (index > len) {
            len += mem_realloc_size;
            str = (char*)realloc(str, (len + 1) * sizeof(char));
            if (str == nullptr) {
                error::send_error(
                    error::MemoryError, language::error::memoryerror);
            }
        }
        str[index] = (char)c;
        c = fgetc(stream);
        index++;
    }
    str[index] = '\0';
    return true;
}

/**
@brief 读入字符串，保存到流中，遇到换行符或EOF结束
*/
void readstr(std::stringstream& s, FILE* in) {
    // clear first
    s.str("");
    s.clear();
    int c = fgetc(in);
    while (c != '\n' && c != EOF) {
        s << c;
        c = fgetc(in);
    }
}
}
