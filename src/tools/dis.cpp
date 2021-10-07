/**
 * 反编译程序，将其反编译成助记符
 */
#include <string>
#include <iostream>
#include "../include/code_loader.h"
#include "../include/ctree_loader.h"
#include "../include/read.h"
#include "../include/Compiler/Compiler.h"
#include "../include/TVM/TVM.h"

using namespace std;

static void out(TVM &vm, string file_name) {
    /**
     * 输出TVM的值
     */
    cout << "From file " << file_name << ":" << "\n";
    // 输出版本号
    cout << "Version:" << vm.static_data.ver_ << "\n";
    // 输出字节码
    cout << "\nCode:\n";
    int line_index = 0;
    for (auto &line : vm.static_data.byte_codes) {
        // 行
        cout << "    " << line_index++ << ":";
        for (auto &value_ : line) {
            cout << int_code[value_[0]] << "|" << value_[1] << ", ";
        }
        cout << "\n";
    }
    size_t n;
    int i;
    // 输出常量池
    // 整型常量池
    // 注意：大整数不在此输出
    n = vm.static_data.const_i.size();
    cout << "\nint constant pool:\n";
    for (i = 0; i < n; ++i) {
        cout << "    " << i << ":" << vm.static_data.const_i[i] << "\n";
    }

    // 浮点数常量池
    n = vm.static_data.const_f.size();
    cout << "\nfloat constant pool:\n";
    for (i = 0; i < n; ++i) {
        cout << "    " << i << ":" << vm.static_data.const_f[i] << "\n";
    }
    // 字符串常量池
    n = vm.static_data.const_s.size();
    cout << "\nstring constant pool:\n";
    for (i = 0; i < n; ++i) {
        cout << "    " << i << ":" << vm.static_data.const_s[i] << "\n";
    }
    // 大整数
    cout << "\nlong int constant pool:\n";
    n = vm.static_data.const_long.size();
    for (i = 0; i < n; ++i) {
        cout << "    " << i << ":" << vm.static_data.const_long[i] << "\n";
    }

    // 输出名字列表
    n = vm.static_data.const_name.size();
    cout << "\nname:\n";
    for (i = 0; i < n; ++i) {
        cout << "    " << i << ":" << vm.static_data.const_name[i] << "\n";
    }
    // 输出函数
    n = vm.static_data.funcs.size();
    cout << "\nfunctions:\n";
    auto itor = vm.static_data.funcs.begin();
    for (i = 0; i < n; ++i, ++itor) {
        cout << "    " << i << ":" << itor->first;
    }
    cout << "\n";
}

void __dis(TVM *vm, const string &file_path) {
    if (is_magic(file_path))
        loader_ctree(vm, file_path);
    else {
        string codes;
        readcode(codes, file_path);
        Compiler(vm, codes);
    }
    out(*vm, file_path);
}

int dis(int argc, char *argv[]) {
    TVM *vm = create_TVM();
    for (int i = 2; i < argc; ++i)
        __dis(vm, argv[i]);
    delete vm;
    return 0;
}
