/**
 * 控制生成参数，如生成编译器参数
 */

module;
#include <cstdlib>
#include <format>
export module generated_params;
import Compiler;
import compile_env;
import compiler_def;
import cmdparser;
import color;
import trcdef;
import help;
import basic_def;
import token;
import trc_flong;
import trc_long;
import data;
import unreach;

namespace trc::tools {
bool gen_number_table = true;
bool optimize = false;
bool const_fold = true;

export compiler::compiler_option compilerOption;

export void argv_lex() {
    // 这里是编译器参数
    int opt;
    cmdparser::option long_options[] = { // 是否生成行号表
        { "noline_number_table", cmdparser::no_argument, nullptr, 't' },
        // 是否进行常量折叠
        { "no_const_fold", cmdparser::no_argument, nullptr, 'f' },
        // 是否优化
        { "optimize", cmdparser::no_argument, nullptr, 'o' },
        // version
        { "version", cmdparser::no_argument, nullptr, 'v' },
        // help
        { "help", cmdparser::no_argument, nullptr, 'h' }, { 0, 0, 0, 0 }
    };
    while (
        (opt = cmdparser::getopt_long(argc, argv, "voh", long_options, nullptr))
        != -1) {
        switch (opt) {
        case 'o': {
            optimize = true;
            break;
        }
        case 'f': {
            const_fold = false;
            break;
        }
        case 't': {
            gen_number_table = false;
            break;
        }
        case 'v': {
            color::green(std::format("Version {}\n", def::version));
            exit(0);
        }
        case 'h': {
            tools_out::help();
            exit(0);
        }
        default: {
            color::red("Trc:Option lex failed!\n");
            exit(1);
        }
        }
    }
    // genarate compiler option
    compilerOption.optimize = optimize;
    compilerOption.const_fold = const_fold;
    compilerOption.number_table = gen_number_table;
}
}
