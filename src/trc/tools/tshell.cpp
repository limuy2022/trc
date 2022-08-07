/**
 * trc交互式界面，无需文件即可执行
 * repl:read-execute-print-loop
 */

#include <Compiler/Compiler.h>
#include <TVM/TVM.h>
#include <TVM/memory.h>
#include <base/Error.h>
#include <base/io.hpp>
#include <csetjmp>
#include <cstdio>
#include <generated_params.h>
#include <string>

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
        if (is_block(temp))
            ++break_num;
        else if (is_end(temp)) {
            --break_num;
            if (!break_num)
                break;
        }
    }
    free(temp);
}

namespace tools::tools_out {
    void tshell() {
        printf("Trc %s\n\n", def::version);

        char* code = nullptr;

        TVM_space::TVM* vm = TVM_space::create_TVM();
        // tshell报错但不终止程序
        error::error_env::quit = false;
        // 解析命令行参数
        auto option = generate_compiler_params();
        // 先传入空代码获取对象
        compiler::detail_compiler* info_saver
            = compiler::Compiler(vm, "", &option, nullptr, true);
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
            if (!setjmp(error::error_env::error_back_place)) {
                compiler::Compiler(vm, code_str, nullptr, info_saver, false);
                vm->reload_data();
                vm->run_all();
            }
        }
        // 该地不需要释放内存的原因是退出只可能是ctrl+c或者exit函数，而退出后会由操作系统回收内存
    }
}
}
