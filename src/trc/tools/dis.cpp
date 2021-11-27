/**
 * 反编译程序，将其反编译成助记符
 */
#include <string>
#include <iostream>
#include "base/code_loader.h"
#include "base/ctree_loader.h"
#include "base/utils/filesys.h"
#include "TVMbase/TVM_data.h"
#include "Compiler/Compiler.h"

using namespace std;

namespace trc {
    namespace tools_in {
        void out(const TVM_space::TVM_data& static_data, const string& file_name) {
            /**
             * 输出TVM的值
             */
            cout << "From file " << file_name << ":" << "\n";
            // 输出版本号
            cout << "Version:" << static_data.ver_ << "\n";
            // 输出字节码
            cout << "\nCode:\n";
            int line_index = 0;
            for (const auto& line : static_data.byte_codes) {
                // 行
                cout << "    " << line_index++ << ":";
                for (const auto& value_ : line) {
                    cout << loader::int_code[value_->bycode] << "|" << value_->index << ", ";
                }
                cout << "\n";
            }
            size_t n;
            int i;
            // 输出常量池
            // 整型常量池
            // 注意：大整数不在此输出
            n = static_data.const_i.size();
            cout << "\nint constant pool:\n";
            for (i = 0; i < n; ++i) {
                cout << "    " << i << ":" << static_data.const_i[i] << "\n";
            }

            // 浮点数常量池
            n = static_data.const_f.size();
            cout << "\nfloat constant pool:\n";
            for (i = 0; i < n; ++i) {
                cout << "    " << i << ":" << static_data.const_f[i] << "\n";
            }
            // 字符串常量池
            n = static_data.const_s.size();
            cout << "\nstring constant pool:\n";
            for (i = 0; i < n; ++i) {
                cout << "    " << i << ":" << static_data.const_s[i] << "\n";
            }
            // 大整数
            cout << "\nlong int constant pool:\n";
            n = static_data.const_long.size();
            for (i = 0; i < n; ++i) {
                cout << "    " << i << ":" << static_data.const_long[i] << "\n";
            }

            // 输出名字列表
            n = static_data.const_name.size();
            cout << "\nname:\n";
            for (i = 0; i < n; ++i) {
                cout << "    " << i << ":" << static_data.const_name[i] << "\n";
            }
            // 输出函数
            n = static_data.funcs.size();
            cout << "\nfunctions:\n";
            auto itor = static_data.funcs.begin();
            for (i = 0; i < n; ++i, ++itor) {
                cout << "    " << i << ":" << itor->first;
            }
            cout << "\n";
        }

        void __dis(TVM_space::TVM *vm, const string &file_path) {
            if (loader::is_magic(file_path))
                loader::loader_ctree(vm, file_path);
            else {
                string codes;
                utils::readcode(codes, file_path);
                compiler::Compiler(vm, codes);
            }
            out(vm -> static_data, file_path);
        }
    }

    namespace tools_out {
        void dis(int argc, char *argv[]) {
            TVM_space::TVM *vm = TVM_space::create_TVM();
            for (int i = 2; i < argc; ++i)
                tools_in::__dis(vm, argv[i]);
            delete vm;
        }
    }
}
