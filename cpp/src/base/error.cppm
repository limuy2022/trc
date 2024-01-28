module;
#include <format>
#include <libintl.h>
#include <string>
export module error;
import trcdef;

export namespace trc::error {
class content {
public:
    content() {
    }

private:
    virtual const char* get_module_name() = 0;
    virtual size_t get_line() = 0;
};

class error_info {
public:
    const char* error_type;
    std::string error_msg;
};

const char* no_reach
    = "The program executed code that should not have been executed.Please "
      "report the problem to the Github repository.";

template <typename T, typename E>
class [[nodiscard("ignore the result")]] Result {

}

class vm_run_error : public std::exception {
public:
    const content* error_content;
    error_info info;
    vm_run_error(const content* error_content, error_info error_if)
        : error_content(error_content)
        , error_info(error_info) {
    }

    const char* what() const noexcept override {
        return "vm run error";
    }

    std::string error_msg() {
        auto ret = std::vformat("\n{}{}\n{}{}:\n");
        fprintf(stderr, , error::error_from, module_name.c_str(),
            error::error_in_line, postion_info.c_str());
        // 输出错误名
        fprintf(stderr, "%s", language::error::error_map[name]);
    }
};

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

const char* error_from = "Error from {}";
const char* error_in_line = "Error in line {}";
}
