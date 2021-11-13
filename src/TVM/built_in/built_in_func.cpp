/**
 * 虚拟机内置函数定义处
 */

#include <iostream>
#include "Error.h"
#include "type.hpp"
#include "TVM/TVM.h"
#include "TVM/TRE.h"
#include "memory/objs_pool.hpp"

using namespace std;

void LENGTH(int argc, TVM *vm) {
    /**
    * 计算长度
    */

    if (argc != 1) {
        send_error(ArgumentError, "len only needs one argnment.");
    }
    vm->pop(vm->firsts);
    vm->push(new trc_int(vm->firsts->len()));
}

void HELP(int argc, TVM *vm) {
    /**
    * 输出帮助
    * 其实这只是个指引，真正的帮助需要查看文档
    */
    cout << "\nWelcome to Tree!If this is your first time using Tree,you can read the Doc to find help.\n";
}

void PRINT(int argc, TVM *vm) {
    /**
    * 打印值
    */
    for (int i = 0; i < argc; ++i) {
        vm->pop(vm->firstv);
        vm->firstv->putline(cout);
    }
}

void PRINTLN(int argc, TVM *vm) {
    /**
    * 打印值并换行
    */
    for (int i = 0; i < argc; ++i) {
        vm->pop(vm->firstv);
        vm->firstv->putline(cout);
    }
    cout << "\n";
}

void INPUT(int argc, TVM *vm) {
    /**
     * 获取输入
     */

    for (int i = 0; i < argc; ++i) {
        vm->pop(vm->firsts);
        vm->firsts->putline(cout);
    }
    cin >> *(vm->firsts);
    vm->push(vm->firsts);
}

void EXIT(int argc, TVM *vm) {
    /**
     * 退出程序
     */
    exit(0);
}

void INT_(int argc, TVM *vm) {
    /**
     * 强制转化为int类型
     */

    vm->pop(vm->firstv);
    vm->push(vm->firstv->to_int());
}

void STRING_(int argc, TVM *vm) {
    /**
     * 强制转化为string类型
     */
    // 利用了写好的底层转化函数，将不是string型的转化为string
    vm->pop(vm->firstv);
    vm->push(vm->firstv->to_string());
}

void FLOAT_(int argc, TVM *vm) {
    /**
     * 强制转化为double（float）类型
     */
    // 利用了写好的底层转化函数，将不是double型的转化为double
    vm->pop(vm->firstv);
    vm->push(vm->firstv->to_float());
}

void BOOL_(int argc, TVM *vm) {
    /**
     * 强制转化为布尔值
     */
    vm->pop(vm->firstv);
    vm->push(vm->firstv->to_bool());
}

void TYPE(int argc, TVM *vm) {
    /**
     * 获取数据类型
     */

    vm->pop(vm->firstv);
    vm->push(MALLOCSTRING(type_int::int_name_s[vm->firstv->gettype()]));
}
