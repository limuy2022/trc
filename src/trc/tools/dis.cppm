/**
 * 反编译程序，将其反编译成助记符
 */

module;
#include <cstdio>
#include <string>
export module dis;
import Compiler;
import code_loader;
import ctree_loader;
import filesys;
import TVM;
import generated_params;
import cmdparser;

namespace trc::tools {
namespace tools_in {
    /**
     * @brief 输出TVM中的数据
     */
    static void out(const TVM_space::TVM& static_data, const char* file_name) {
        printf("From file %s:\n", file_name);
        // 输出字节码
        puts("\nCode:");
        for (const auto& bycode : static_data.static_data.byte_codes) {
            // 行
            printf("    %zu:%s|%hd, \n",
                static_data.static_data
                    .line_number_table[static_data.run_index],
                loader::int_code[(unsigned int)bycode.bycode], bycode.index);
        }
        size_t n;
        // 输出常量池
        // 整型常量池
        // 注意：大整数不在此输出
        n = static_data.static_data.const_i.size();
        puts("\nint constant pool:");
        for (size_t i = 0; i < n; ++i) {
            printf("    %zu:%d\n", i, static_data.static_data.const_i[i]);
        }

        // 浮点数常量池
        n = static_data.static_data.const_f.size();
        puts("\nfloat constant pool:");
        for (size_t i = 0; i < n; ++i) {
            printf("    %zu:%lf\n", i, static_data.static_data.const_f[i]);
        }
        // 字符串常量池
        n = static_data.static_data.const_s.size();
        puts("\nstring constant pool:");
        for (size_t i = 0; i < n; ++i) {
            printf(
                "    %zu:%s\n", i, static_data.static_data.const_s[i].c_str());
        }
        // 大整数
        puts("\nlong int constant pool:");
        n = static_data.static_data.const_long.size();
        for (size_t i = 0; i < n; ++i) {
            printf("    %zu:%s\n", i,
                static_data.static_data.const_long[i].c_str());
        }
        // 输出函数
        puts("\nfunctions:");
        n = static_data.static_data.funcs.size();
        for (size_t i = 0; i < n; ++i) {
            printf("    %zu:%s\n", i, static_data.static_data.funcs[i].name);
        }
        putchar('\n');
    }

    void _dis(TVM_space::TVM* vm, const char* file_path) {
        if (loader::is_magic(file_path))
            loader::loader_ctree(vm, file_path);
        else {
            std::string codes;
            utils::readcode(codes, file_path);
            compiler::Compiler(compiler::main_module, tools::compilerOption, vm->static_data)
                .compile(codes);
        }
        out(*vm, file_path);
    }
}

namespace tools_out {
    export void dis() {
        auto vm = new TVM_space::TVM;
        for (int i = cmdparser::optind + 1; i < argc; ++i) {
            tools_in::_dis(vm, argv[i]);
        }
        delete vm;
    }
}
}
