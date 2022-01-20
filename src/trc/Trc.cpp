/**
 * File    :   Trc.cpp
 * Time    :   2021/07/09 20:49:12
 * Author  :   李沐阳
 * @warning 此文件不能包含任何需要测试的程序
 */

#include "base/color.h"
#include "base/memory/memory.h"
#include "base/trcdef.h"
#include "base/utils/data.hpp"
#include "gflags/gflags.h"
#include "language/language.h"
#include "tools.h"
#include <cstdio>
#include <cstring>

/**
 * @brief 报出找不到模式错误
 */
static inline void show_error(const char* mode) {
    trc::color::red.print(
        "\"%s\"%s", mode, language::trc::mode_not_found);
}

/**
 * @brief 输出版本号，version命令行参数对应的函数
 */
static void showversion() {
    printf("The version is %.1f\n", trc::def::version);
}

typedef void (*argv_func_tools)();

static const char* tools_func_name[] = {
    "tdb",
    "help",
    "version",
    "run",
    "token",
    "dis",
    "grammar",
    "brun",
    "build",
};

static const argv_func_tools tools_func[]
    = { trc::tools::tools_out::tdb,
          trc::tools::tools_out::help, showversion,
          trc::tools::tools_out::run,
          trc::tools::tools_out::out_token,
          trc::tools::tools_out::dis,
          trc::tools::tools_out::out_grammar,
          trc::tools::tools_out::brun,
          trc::tools::tools_out::build };

/**
 * @brief 查找对应工具并运行
 */
static inline void run_yes_argv_mode(char* mode) {
    for (int i = 0,
             end
         = trc::utils::sizeof_static_array(tools_func);
         i < end; ++i) {
        if (!strcmp(mode, tools_func_name[i])) {
            // 匹配上了
            tools_func[i]();
            return;
        }
    }
    show_error(mode);
}

int main(int argc, char* argv[]) {
    trc::memory::init_mem();
    // 设置命令行参数
    trc::tools::argv = argv;
    trc::tools::argc = argc;
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    if (argc == 1) {
        // 不指定模式，没有参数，默认为交互模式
        trc::tools::tools_out::tshell();
    } else {
        // 指定模式，匹配调用模式
        run_yes_argv_mode(argv[1]);
    }
    // 执行正常情况下卸载内存，不正常的话就崩溃了
    trc::memory::quit_mem();
    return 0;
}
