/**
 * trc交互式界面，无需文件即可执行
 * repl:read-execute-print-loop
 */

module;
#include <cstdio>
#include <string>
export module tshell;
import Compiler;
import TVM;
import Error;
import io;
import generated_params;
import trcdef;

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
    char* temp = nullptr;
    int break_num = 1;
    for (;;) {
        for (int i = 0; i <= break_num; ++i)
            printf("%s", "    ");
        printf("->");
        free(temp);
        io::readstr(temp, stdin);
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
    free(temp);
}

namespace tools::tools_out {
    /**
     * @brief trc的交互式终端界面
     */
    export void tshell() {
        printf("Trc %s\n\n", def::version);

        char* code = nullptr;

        auto vm = new TVM_space::TVM;
        // tshell报错但不终止程序
        error::error_env::quit = false;
        // 先传入空代码获取对象
        compiler::Compiler info_saver(
            compiler::main_module, tools::compilerOption, vm->static_data);
        for (;;) {
            printf("%s", "\ntshell>");
            free(code);
            io::readstr(code, stdin);
            std::string code_str(code);
            if (is_block(code)) {
                get_block(code_str);
            }
            vm->static_data.byte_codes.clear();
            // 设置好报错时返回到的地址
            try{
                info_saver.compile(code);
                vm->reload_data();
                vm->run_all();
            } catch(error::error_env::vm_run_error) {

            }
            // 重新还原行号
            info_saver.compiler_data.error.reset_line();
        }
        // 该地不需要释放内存的原因是退出只可能是ctrl+c或者exit函数，而退出后会由操作系统回收内存
    }
}
}
