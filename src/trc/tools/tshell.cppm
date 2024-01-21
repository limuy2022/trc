/**
 * trc交互式界面，无需文件即可执行
 * repl:read-execute-print-loop
 */

module;
#include <cstdio>
#include <sstream>
#include <string>
export module tshell;
import TVM;
import Error;
import io;
import generated_params;
import trcdef;
import compile_env;
import compiler_def;
import color;
import help;
import compiler;

/**
 * @brief 判断是否为新的语句块开始
 */
static inline bool is_block(const std::string& code) {
    return !code.empty() && code.back() == '{';
}

/**
 * @brief 判断语句块是否结束
 */
static inline bool is_end(const std::string& code) {
    return !code.empty() && code.back() == '}';
}

namespace trc {
/**
 * @brief
 * 由于整个语句块必须一起传递给token_lex解析，一起收集数据并且更改命令提示标志为->
 * @param res 语句块储存
 */
static void get_block(std::string& res) {
    int break_num = 1;
    for (;;) {
        std::string temp;
        for (int i = 0; i <= break_num; ++i)
            printf("%s", "    ");
        printf("->");
        std::getline(std::cin, temp);
        res += temp;
        res += '\n';
        if (is_block(temp)) {
            ++break_num;
        } else if (is_end(temp)) {
            --break_num;
            if (break_num == 0) {
                break;
            }
        }
    }
}



namespace tools::tools_out {
    /**
     * @brief trc的交互式终端界面
     */
    export void tshell() {
        printf("Trc %s\n\n", def::version);

        auto vm = new TVM_space::TVM;
        // tshell报错但不终止程序
        error::error_env::quit = false;
        for (;;) {
            printf("%s", "\ntshell>");
            auto tmpf = tmpfile();
            std::string code_str;
            std::getline(std::cin, code_str);
            if (is_block(code_str)) {
                get_block(code_str);
            }
            fprintf(tmpf, "%s", code_str.c_str());
            rewind(tmpf);
            vm->static_data.byte_codes.clear();
            try {
                compiler::compiler().parse(tools::compilerOption, "tshell", tmpf, &vm->static_data);
                vm->reload_data();
                vm->run_all();
            } catch (error::error_env::vm_run_error) { }
            fclose(tmpf);
        }
    }
}
}
