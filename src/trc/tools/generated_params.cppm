/**
 * 控制生成参数，如生成编译器参数
 */

module;
#include <cmdparser.hpp>
export module generated_params;
import Compiler;
import compiler_def;

export namespace trc::tools {
cli::Parser* parser = nullptr;
char**argv;
int argc;

void init_parser() {
    // 这里是编译器参数
    // 是否优化
    parser->set_optional<bool>("o", "optimize", false,
        "Controls whether optimization code is generated.");
    // 是否生成行号表
    parser->set_optional<bool>(
        "nt", "noline_number_table", false, "Controls whether a line number.");
    // 是否进行常量折叠
    parser->set_optional<bool>("nf", "no_const_fold", false,
        "Controls whether optimization includes costant fold.");
}

/**
 * @brief 生成编译器参数
 */
compiler::compiler_option generate_compiler_params() {
    // 取反是为了更符合使用习惯
    return compiler::compiler_option { !parser->get<bool>("nt"),
        parser->get<bool>("o"), !parser->get<bool>("nf") };
}

/**
 * @brief 该文件用于按顺序获取非-xxx参数
 */
class argv_parser{
public:
    /**
     * @return nullptr代表解析到尽头
     */
    char* next() {
        if(index >= argc) {
            return nullptr;
        }
        return argv[index++];
    }
private:
    // 直接指向trc xxx后的参数
    int index = 2;
}default_argv_parser;
}
