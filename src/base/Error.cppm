module;
#include <language/error.hpp>
#include <string>
export module Error;
import trcdef;

export namespace trc::error {
/**
 * 报错设置
 * 系统需要知道当前处于什么模式，以合适的模式应对发生的状况
 */
namespace error_env {
    // 是否终止程序
    bool quit = true;

    class vm_run_error:public std::exception{};
}

// 错误，增强可读性
enum error_type {
    NameError,
    ValueError,
    SyntaxError,
    VersionError,
    OpenFileError,
    ModuleNotFoundError,
    ArgumentError,
    ZeroDivError,
    RunError,
    AssertError,
    IndexError,
    MemoryError,
    KeyError,
    SystemError,
    OperatorError,
    RedefinedError
};

/**
 * @brief 输出报错信息
 * @param error_name 异常名
 * @param ap 可变参数
 */
template <typename... argv_t>
void output_error_msg(error_type error_name, const argv_t&... ap) {
    // 报错的模板字符串
    auto index = 0;
    const char* argv_arr[] = { ap... };
    const char* base_string = argv_arr[0];
    for (size_t i = 0; base_string[i]; ++i) {
        if (base_string[i] == '%') {
            // 输出报错字符串
            fprintf(stderr, "%s", argv_arr[++index]);
        } else {
            fputc(base_string[i], stderr);
        }
    }
    fputc('\n', stderr);
}

template <typename... argv_t>
void send_error_detail(error_type name, const std::string& module_name,
    size_t line_index, const argv_t&... ap) {
    fprintf(stderr, "\n%s%s\n%s%zu:\n", language::error::error_from,
        module_name.c_str(), language::error::error_in_line, line_index);
    // 输出错误名
    fprintf(stderr, "%s", language::error::error_map[name]);
    output_error_msg(name, ap...);
    // 检查设置判断是否报错
    if (error_env::quit) {
        // 报错，退出程序
        exit(EXIT_FAILURE);
    }
    // 跳转到执行的地方
    throw error_env::vm_run_error();
}

template <typename... argv_t>
void send_error(error_type error_name, const argv_t&... argv) {
    send_error_detail(error_name, "__main__", 0, argv...);
}

/**
 * @brief 关于模块报错的异常封装
 * @details 包含模块名和当前行数
 */
class error_module {
public:
    // 模块的名字
    const std::string name;

    explicit error_module(std::string name)
        : name(std::move(name)) {
    }
    /**
     * @brief 报出错误
     */
    template <typename... P>
    void send_error_module(error_type errorn, const P&... argv) {
        send_error_detail(errorn, name, line, argv...);
    }

    [[nodiscard]] line_t get_line() const {
        return line;
    }

    void add_line() {
        line++;
    }

    void sub_line() {
        line--;
    }

    void reset_line() {
        line = 1;
    }

private:
    // 当前操作的行号
    line_t line = 1;
};
}
