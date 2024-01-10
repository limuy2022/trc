/**
 * File    :   Trc.cpp
 * Time    :   2021/07/09 20:49:12
 * Author  :   李沐阳
 */

#include <cstring>
#include <format>
#include <libintl.h>
#include <clocale>
#ifdef UNITTEST
#include <gtest/gtest.h>
#endif
import generated_params;
import TVM.memory;
import color;
import memory;
import trcdef;
import build;
import dis;
import cmdparser;
import help;
import run;
import style;
import tdb;
import tools.token;
import tshell;
import basic_def;
import data;
import color;
import help;
import Compiler;
import ctree_loader;
import filesys;
import code_loader;
import io;

namespace trc {
/**
 * @brief 报出找不到模式错误
 */
static inline void show_error(const char* mode) {
    color::red(
        std::format("Trc:\"{}\"{}", mode, language::trc::mode_not_found));
}

// 命令函数的接口类型
typedef void (*argv_func_tools)();

struct {
    const char* name;
    argv_func_tools tool_func;
} cmd_tool[] = { { "tdb", tools::tools_out::tdb },
    { "help", tools::tools_out::help }, { "run", tools::tools_out::run },
    { "token", tools::tools_out::out_token }, { "dis", tools::tools_out::dis },
    { "build", tools::tools_out::build },
    { "style", tools::tools_out::style } };

/**
 * @brief 查找对应工具并运行
 * @param mode 对应工具
 * unittest时不会被使用
 */
[[maybe_unused]] static inline void find_mode_to_run(const char* mode) {
    for (const auto& i : cmd_tool) {
        if (!strcmp(mode, i.name)) {
            // 匹配上了
            i.tool_func();
            return;
        }
    }
    show_error(mode);
}

/**
 * @brief 释放内存，程序结束时使用
 */
static inline void quit_mem() {
    TVM_space::TVM_quit_mem();
}
}

int main(int argc, char* argv[]) {
    // 初始化系统
    // 初始化地域化设置
    std::setlocale(LC_ALL, "");
    bindtextdomain("trans", "locales");
    textdomain("trans");
/*控制台初始化*/
#ifdef _WIN32
    trc::color::console_init();
#endif
#ifndef UNITTEST
    /* 内存初始化*/
    trc::memory::init_mem();
    /* 初始化命令行解析器*/
    trc::tools::argc = argc;
    trc::tools::argv = argv;
    trc::tools::argv_lex();
    if (trc::cmdparser::optind >= argc) {
        // 不指定模式，没有参数，默认为交互模式
        trc::tools::tools_out::tshell();
    } else {
        // 指定模式，匹配调用模式
        trc::find_mode_to_run(argv[trc::cmdparser::optind]);
    }
#else
    ::testing::GTEST_FLAG(output) = "xml:unittest.xml";
    // 启动单元测试
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
#endif
    // 卸载系统
    // 卸载内存，做收尾工作
    trc::quit_mem();
    return 0;
}
