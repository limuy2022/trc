/**
 * 虚拟机内置函数定义处
 */

#include <TVM/TRE.h>
#include <TVM/TVM.h>
#include <TVM/memory.h>
#include <TVM/types/trc_string.h>
#include <base/Error.h>
#include <base/memory/objs_pool.hpp>
#include <cstdio>
#include <language/error.h>

namespace trc::TVM_space::builtin {
void LENGTH(int argc, TVM* vm) {
    if (argc != 1) {
        vm->error_report(
            error::ArgumentError, language::error::argumenterror, "len", "1");
    }
    auto firsts = (def::STRINGOBJ)vm->pop();
    vm->push(new types::trc_int(firsts->len()));
}

void HELP([[maybe_unused]] int argc, [[maybe_unused]] TVM* vm) {
    puts("\nWelcome to Tree!If this is your "
         "first time using Trcs,you "
         "can read the Doc to find help.");
}

void PRINT(int argc, TVM* vm) {
    for (int i = 0; i < argc; ++i) {
        vm->pop()->putline(stdout);
    }
}

void PRINTLN(int argc, TVM* vm) {
    for (int i = 0; i < argc; ++i) {
        vm->pop()->putline(stdout);
    }
    putchar('\n');
}

void INPUT(int argc, TVM* vm) {
    for (int i = 0; i < argc; ++i) {
        ((def::STRINGOBJ)vm->pop())->putline(stdout);
    }
    def::STRINGOBJ strtmp = MALLOCSTRING();
    strtmp->in(stdin);
    vm->push(strtmp);
}

void EXIT(int argc, TVM* vm) {
    exit(0);
}

void INT_(int argc, TVM* vm) {
    vm->top() = vm->top()->to_int();
}

void STRING_(int argc, TVM* vm) {
    // 利用了写好的底层转化函数，将不是string型的转化为string
    vm->top() = vm->top()->to_string();
}

void FLOAT_(int argc, TVM* vm) {
    // 利用了写好的底层转化函数，将不是double型的转化为double
    vm->top() = vm->top()->to_float();
}

void BOOL_(int argc, TVM* vm) {
    vm->top() = vm->top()->to_bool();
}

void TYPE(int argc, TVM* vm) {
    vm->top() = MALLOCSTRING(type_int::int_name_s[(int)vm->top()->gettype()]);
}
}
