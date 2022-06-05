/**
 * 虚拟机内置函数定义处
 */

#include <TVM/TVM.h>
#include <TVMbase/TRE.h>
#include <TVMbase/memory.h>
#include <TVMbase/types/string.h>
#include <base/Error.h>
#include <base/memory/objs_pool.hpp>
#include <cstdio>
#include <language/error.h>

namespace trc::TVM_space::builtin {
void LENGTH(int argc, TVM* vm) {
    if (argc != 1) {
        vm->error_report(error::ArgumentError,
            language::error::argumenterror, "len", "1");
    }
    firsts = (def::STRINGOBJ)vm->pop();
    vm->push(new types::trc_int(firsts->len()));
}

void HELP(int argc, TVM* vm) {
    puts("\nWelcome to Tree!If this is your "
         "first time using Tree,you "
         "can read the Doc to find help.");
}

void PRINT(int argc, TVM* vm) {
    for (int i = 0; i < argc; ++i) {
        firstv = vm->pop();
        firstv->putline(stdout);
    }
}

void PRINTLN(int argc, TVM* vm) {
    for (int i = 0; i < argc; ++i) {
        firstv = vm->pop();
        firstv->putline(stdout);
    }
    putchar('\n');
}

void INPUT(int argc, TVM* vm) {
    for (int i = 0; i < argc; ++i) {
        firsts = (def::STRINGOBJ)vm->pop();
        firsts->putline(stdout);
    }
    firsts->in(stdin);
    vm->push(firsts);
}

void EXIT(int argc, TVM* vm) {
    exit(0);
}

void INT_(int argc, TVM* vm) {
    firstv = vm->pop();
    vm->push(firstv->to_int());
}

void STRING_(int argc, TVM* vm) {
    // 利用了写好的底层转化函数，将不是string型的转化为string
    firstv = vm->pop();
    vm->push(firstv->to_string());
}

void FLOAT_(int argc, TVM* vm) {
    // 利用了写好的底层转化函数，将不是double型的转化为double
    firstv = vm->pop();
    vm->push(firstv->to_float());
}

void BOOL_(int argc, TVM* vm) {
    firstv = vm->pop();
    vm->push(firstv->to_bool());
}

void TYPE(int argc, TVM* vm) {
    firstv = vm->pop();
    vm->push(MALLOCSTRING(
        type_int::int_name_s[(int)firstv->gettype()]));
}
}
