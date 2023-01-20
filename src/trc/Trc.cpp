/**
 * File    :   Trc.cpp
 * Time    :   2021/07/09 20:49:12
 * Author  :   李沐阳
 */

#include <TVM/memory.hpp>
#include <base/color.hpp>
#include <base/memory/memory.hpp>
#include <cstring>
#include <gflags/gflags.h>
#include <language/language.hpp>
#include <platform.hpp>
#include <tools.hpp>
#ifdef UNITTEST
#include <gtest/gtest.h>
#endif

namespace trc {
/**
 * @brief 报出找不到模式错误
 */
static inline void show_error(const char* mode) {
    color::red("Trc:\"%s\"%s", mode, language::trc::mode_not_found);
}

/**
 * @brief 输出版本号，version命令行参数对应的函数
 */
static void showversion() {
    color::green("Version %s\n", def::version);
}

// 命令函数的接口类型
typedef void (*argv_func_tools)();

struct {
    const char* name;
    argv_func_tools tool_func;
} cmd_tool[] = { { "tdb", tools::tools_out::tdb },
    { "help", tools::tools_out::help }, { "version", showversion },
    { "run", tools::tools_out::run }, { "token", tools::tools_out::out_token },
    { "dis", tools::tools_out::dis }, { "brun", tools::tools_out::brun },
    { "build", tools::tools_out::build },
    { "style", tools::tools_out::style } };

/**
 * @brief 查找对应工具并运行
 * @param mode 对应工具
 */
static inline void find_mode_to_run(char* mode) {
    for (auto i : cmd_tool) {
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
    language::locale_init();
/*控制台初始化*/
#ifdef WINDOWS_PLAT
    trc::color::console_init();
#endif
#ifndef UNITTEST
    /* 内存初始化*/
    trc::memory::init_mem();
    /* 初始化命令行解析器*/
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    /* 设置命令行参数*/
    trc::tools::argv = argv;
    trc::tools::argc = argc;
    if (argc == 1) {
        // 不指定模式，没有参数，默认为交互模式
        trc::tools::tools_out::tshell();
    } else {
        // 指定模式，匹配调用模式
        trc::find_mode_to_run(argv[1]);
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
