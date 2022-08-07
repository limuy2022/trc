/**
 * 反编译程序，将其反编译成助记符
 */

#include <Compiler/Compiler.h>
#include <base/code_loader.h>
#include <base/ctree_loader.h>
#include <base/utils/filesys.h>
#include <cstdio>
#include <generated_params.h>
#include <string>
#include <tools.h>

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
                loader::int_code[bycode.bycode], bycode.index);
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
            printf("    %zu:%s\n", i, static_data.static_data.const_s[i]);
        }
        // 大整数
        puts("\nlong int constant pool:");
        n = static_data.static_data.const_long.size();
        for (size_t i = 0; i < n; ++i) {
            printf("    %zu:%s\n", i, static_data.static_data.const_long[i]);
        }
        // 输出函数
        n = static_data.static_data.funcs.size();
        puts("\nfunctions:");
        auto itor = static_data.static_data.funcs.begin();
        for (size_t i = 0; i < n; ++i, ++itor) {
            printf("    %zu:%s\n", i, itor->first.c_str());
        }
        putchar('\n');
    }

    void __dis(TVM_space::TVM* vm, const char* file_path) {
        if (loader::is_magic(file_path))
            loader::loader_ctree(vm, file_path);
        else {
            std::string codes;
            utils::readcode(codes, file_path);
            auto option = generate_compiler_params();
            compiler::Compiler(vm, codes, &option);
        }
        out(*vm, file_path);
    }
}

namespace tools_out {
    void dis() {
        TVM_space::TVM* vm = TVM_space::create_TVM();
        for (int i = 2; i < argc; ++i)
            tools_in::__dis(vm, argv[i]);
        delete vm;
    }
}
}
