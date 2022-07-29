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

// 检查并重新申请内存
#define CHECK_AND_REALLOC_ERROR_MSG_STR(will_added_len)                                            \
    do {                                                                                           \
        if (used_size >= finally_out_length) {                                                     \
            /* 需要的内存已经超过了申请的，之所以是等于，是因为还有\0 */ \
            /* 多申请5个作预留 */                                                           \
            finally_out_length += (will_added_len) + 5;                                            \
            finally_out = (char*)realloc(finally_out, finally_out_length);                         \
            if (finally_out == nullptr) {                                                          \
                error::send_error(                                                                 \
                    error::MemoryError, language::error::memoryerror);                             \
            }                                                                                      \
        }                                                                                          \
    } while (0)

char* make_error_msg(int error_name, va_list& ap) {
    // 报错的模板字符串
    const char* base_string = va_arg(ap, const char*);
    // 模板字符串的长度
    size_t base_string_len = strlen(base_string);
    // 错误名的长度
    size_t error_name_map_len = strlen(language::error::error_map[error_name]);
    // finally_out增加的大小，大于等于base_string加上错误名的长度,且预留\0后再多预留4个字节
    size_t finally_out_length = error_name_map_len + base_string_len + 5;
    // 具体报错数据储存容器
    char* finally_out = (char*)malloc(finally_out_length * sizeof(char));
    if (finally_out == nullptr) {
        error::send_error(error::MemoryError, language::error::memoryerror);
    }
    // 将错误名拷贝到报错数据中
    strcpy(finally_out, language::error::error_map[error_name]);
    // 已使用长度
    size_t used_size = error_name_map_len;
    // 字符串索引
    size_t res_string_index = error_name_map_len;
    for (size_t i = 0; i < base_string_len; ++i) {
        switch (base_string[i]) {
        case '%': {
            // 插入字符串
            const char* addtmp = va_arg(ap, const char*);
            size_t addtmp_len = strlen(addtmp);
            used_size += addtmp_len;
            CHECK_AND_REALLOC_ERROR_MSG_STR(addtmp_len);
            // 不用strcat是因为效率更高一点
            strncpy(finally_out + res_string_index, addtmp, addtmp_len);
            res_string_index += addtmp_len;
            break;
        }
        case '#': {
            // 插入字符
            char addchar = va_arg(ap, char);
            used_size++;
            CHECK_AND_REALLOC_ERROR_MSG_STR(1);
            finally_out[res_string_index] = addchar;
            res_string_index++;
            break;
        }
        default: {
            finally_out[res_string_index] = base_string[i];
            res_string_index++;
            used_size++;
        }
        }
    }
    finally_out[used_size] = '\0';
    return finally_out;
}

/**
 * @brief 报错的实现细节
 * @details 运用默认参数巧妙解决了两个版本的报错
 */
static void send_error_detail(int name, va_list& ap,
    const std::string& module = "__main__", size_t line_index = 0) {
    char* error_msg = make_error_msg(name, ap);
    send_error_(error_msg, module.c_str(), line_index);
    free(error_msg);
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

void send_error_(
    const char* error_msg, const char* module, size_t line_index) noexcept {
    fprintf(stderr, "\n%s%s\n%s%zu:\n%s\n", language::error::error_from, module,
        language::error::error_in_line, line_index, error_msg);
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
