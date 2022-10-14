#include <algorithm>
#include <base/Error.h>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <language/error.h>
#include <string>

namespace trc::error {
namespace error_env {
    bool quit = true;
    jmp_buf error_back_place;
}

void make_error_msg(int error_name, va_list& ap) {
    // 输出错误名
    fprintf(stderr, "%s", language::error::error_map[error_name]);
    // 报错的模板字符串
    const char* base_string = va_arg(ap, const char*);
    for (size_t i = 0; base_string[i]; ++i) {
        if (base_string[i] == '%') {
            // 输出报错字符串
            fprintf(stderr, "%s", va_arg(ap, const char*));
        } else {
            fputc(base_string[i], stderr);
        }
    }
    fputc('\n', stderr);
}

/**
 * @brief 报错的实现细节
 * @details 运用默认参数巧妙解决了两个版本的报错
 */
static void send_error_detail(int name, va_list& ap,
    const std::string& module = "__main__", size_t line_index = 0) {
    send_error_(module.c_str(), line_index);
    make_error_msg(name, ap);
    // 检查设置判断是否报错
    if (error_env::quit) {
        // 报错，退出程序
        exit(EXIT_FAILURE);
    }
    // 跳转到执行的地方
    longjmp(error_env::error_back_place, 1);
}

void send_error_module_aplist(
    int name, const std::string& module, size_t line_index, va_list& ap) {
    send_error_detail(name, ap, module, line_index);
}

void send_error(int name, ...) {
    va_list ap;
    va_start(ap, name);
    send_error_detail(name, ap);
    va_end(ap);
}

void send_error_(const char* module, size_t line_index) noexcept {
    fprintf(stderr, "\n%s%s\n%s%zu:\n", language::error::error_from, module,
        language::error::error_in_line, line_index);
}

error_module::error_module(std::string name)
    : name(std::move(name)) {
}

void error_module::send_error_module(int ename, ...) {
    va_list ap;
    va_start(ap, ename);
    // 加1是为了从索引转成行号
    error::send_error_module_aplist(ename, name, line + 1, ap);
    va_end(ap);
}
}
