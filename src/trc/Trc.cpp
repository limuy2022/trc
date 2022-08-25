/**
 * File    :   Trc.cpp
 * Time    :   2021/07/09 20:49:12
 * Author  :   李沐阳
 */

#include <TVM/memory.h>
#include <base/color.h>
#include <base/easter_eggs.h>
#include <base/memory/memory.h>
#include <base/trcdef.h>
#include <base/utils/data.hpp>
#include <cstring>
#include <easyloggingpp/easylogging++.h>
#include <gflags/gflags.h>
#include <language/language.h>
#include <platform.h>
#include <tools.h>
#ifdef UNITTEST
#include <gtest/gtest.h>
#endif

INITIALIZE_EASYLOGGINGPP

/**
 * @brief 报出找不到模式错误
 */
static inline void show_error(const char* mode) {
    trc::color::red("Trc:\"%s\"%s", mode, language::trc::mode_not_found);
}

/**
 * @brief 输出版本号，version命令行参数对应的函数
 */
static void showversion() {
    trc::color::green("Version %s\n", trc::def::version);
    // 调用小彩蛋模块
    trc::eggs::canvas_data data = { { 0, 1, 0 }, { 1, 2, 1 }, { 0, 1, 0 } };
    trc::eggs::draw_pictures(data);
}

// 命令函数的接口类型
typedef void (*argv_func_tools)();

static const char* tools_func_name[] = { "tdb", "help", "version", "run",
    "token", "dis", "grammar", "brun", "build", "style" };

static const argv_func_tools tools_func[] = { trc::tools::tools_out::tdb,
    trc::tools::tools_out::help, showversion, trc::tools::tools_out::run,
    trc::tools::tools_out::out_token, trc::tools::tools_out::dis,
    trc::tools::tools_out::out_grammar, trc::tools::tools_out::brun,
    trc::tools::tools_out::build, trc::tools::tools_out::style };

/**
 * @brief 查找对应工具并运行
 * @param mode 对应工具
 */
static inline void find_mode_to_run(char* mode) {
    for (int i = 0, end = trc::utils::sizeof_static_array(tools_func); i < end;
         ++i) {
        if (!strcmp(mode, tools_func_name[i])) {
            // 匹配上了
            tools_func[i]();
            return;
        }
    }
    show_error(mode);
}

/**
 * @brief 释放内存，程序结束时使用
 */
inline static void quit_mem() {
    trc::TVM_space::TVM_quit_mem();
}

/**
 * @brief 设置日志
 */
static void ConfigureLogger() {
    el::Configurations defaultConf;
    defaultConf.setToDefault();
    defaultConf.setGlobally(el::ConfigurationType::Enabled, "true");
    defaultConf.setGlobally(el::ConfigurationType::ToFile, "false");
    defaultConf.setGlobally(el::ConfigurationType::ToStandardOutput, "true");
    defaultConf.setGlobally(el::ConfigurationType::SubsecondPrecision, "6");
    defaultConf.setGlobally(el::ConfigurationType::PerformanceTracking, "true");
    defaultConf.setGlobally(el::ConfigurationType::LogFlushThreshold, "1");
    el::Loggers::reconfigureAllLoggers(defaultConf);
}

int main(int argc, char* argv[]) {
    // 初始化系统
    // 初始化地域化设置
    language::locale_init();
/*控制台初始化*/
#ifdef WINDOWS_PLAT
    trc::color::console_init();
#endif
    // 日志器初始化
    ConfigureLogger();
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
        find_mode_to_run(argv[1]);
    }
#else
    // 启动单元测试
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
#endif
    // 卸载系统
    // 卸载内存，做收尾工作
    quit_mem();
    return 0;
}
