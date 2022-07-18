#pragma once

#include <base/library.h>
#include <csetjmp>
#include <string>

/**
 * @brief 生成符号不匹配报错信息的宏
 * @param t1 参与类型1
 * @param t2 参与类型2
 * @param operator_node 参与运算的运算符的节点
 */
#define OPERERROR_MSG(t1, t2, operator_node)                                   \
    error::OperatorError, language::error::operatorerror,                      \
        str_token_ticks_cal_map                                                \
            [((node_base_tick_without_sons*)(operator_node))->tick],           \
        str_grammar_type_cal_map[(t1)], str_grammar_type_cal_map[(t2)]

namespace trc::error {
/**
 * 报错设置
 * 系统需要知道当前处于什么模式，以合适的模式应对发生的状况
 */
namespace error_env {
    // 是否终止程序
    TRC_base_c_api bool quit;
    // 当quit被设置为false时，会跳转到该地址
    TRC_base_c_api jmp_buf error_back_place;
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
    OperatorError
};

/**
 * @brief 关于模块报错的异常封装
 * @details 包含模块名和当前行数
 */
class TRC_base_api error_module {
public:
    // 模块的名字
    const std::string name;

    // 当前操作的行号
    size_t line = 0;

    error_module(std::string name);

    /**
     * @brief 报出错误
     * @warning
     * 封装该函数的函数不需要转索引，索引在该函数转换
     */
    void send_error_module(int error, ...);
};

/**
 * @brief 报出错误，错误名称和错误信息
 * @warning 一般报错，不带模块和行号
 * @param name 错误名
 * @param 可变参数 const char* 类型的字符串
 * @code error::send_error(error::ValueError, "%
 * can't be %", "int", "string")
 * @endcode
 */
TRC_base_c_api void send_error(int name, ...);

/**
 * @brief 报出有模块有行号的异常
 * @details
 * 高级报错，用于编译和虚拟机运行等，包含模块和行号，注意这两种报错的不同点，该种报错因为需要封装，所以接受va_list类型
 * @code
 * error::send_error_module(error::ValueError,"__main__",
 * 0, "% can't be
 * %", "int", "string") @endcode
 */
TRC_base_c_api void send_error_module_aplist(
    int name, const std::string& module, size_t line_index, va_list& ap);

/**
 * @brief 通用函数，填充报错信息
 * @param error_name 异常名
 * @param ap 可变参数
 * @warning 使用完请释放，并且使用free释放！
 */
TRC_base_c_api char* make_error_msg(int error_name, va_list& ap);

/**
 * @brief 输出报错信息
 * @warning
 * 不接收可变参数，接受已经处理好的报错信息和数据
 * @details 格式化报错信息，规则：%代表插入的是字符串，#代表插入的是字符
 */
TRC_base_c_api void send_error_(
    const char* error_msg, const char* module, size_t line_index) noexcept;
}
