#ifndef TRC_INCLUDE_ERROR_H
#define TRC_INCLUDE_ERROR_H

#include <string>

// 错误宏，增强可读性
#define NameError 0
#define ValueError 1
#define TypeError 2
#define SyntaxError 3
#define VersionError 4
#define OpenFileError 5
#define ModuleNotFoundError 6
#define ArgumentError 7
#define ZeroDivError 8
#define RunError 9
#define AssertError 10
#define IndexError 11
#define MemoryError 12
#define KeyError 13

using namespace std;

namespace error_env {
    /**
     * 报错系统需要知道当前处于什么模式，合理应对发生的状况
     */
    extern bool quit;
    extern bool out_msg;
}

void send_error(const int name, ...);

void error_type(const int name, const string &err_msg, int err_value_type);

string make_error_msg(const int error_name, va_list &ap);

void send_error_(const string &error_msg) noexcept;

#endif
