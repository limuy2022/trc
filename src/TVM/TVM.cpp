#include <TVM/TVM.h>
#include <TVM/func.h>
#include <TVM/memory.h>
#include <base/Error.h>
#include <base/trcdef.h>
#include <cstdarg>
#include <utility>

namespace trc::TVM_space {

TVM::TVM(std::string name)
    : name(std::move(name)) {
    // 初始化TVM
    TVM_space::init_mem();
}

void TVM::reload_data() {
    dyna_data.reset_global_symbol_table(static_data.global_symbol_table_size);
}

void TVM::run_bycode(TVM_bytecode* bycode) {
    switch (bycode->bycode) {
    case 0: {
        LOAD_INT(bycode->index);
        break;
    }
    case 1: {
        ADD();
        break;
    }
    case 2: {
        NOP();
        break;
    }
    case 3: {
        SUB();
        break;
    }
    case 4: {
        MUL();
        break;
    }
    case 5: {
        DIV();
        break;
    }
    case 6: {
        GOTO(bycode->index);
        break;
    }
    case 7: {
        STORE_NAME(bycode->index);
        break;
    }
    case 8: {
        LOAD_NAME(bycode->index);
        break;
    }
    case 9: {
        LOAD_FLOAT(bycode->index);
        break;
    }
    case 10: {
        LOAD_STRING(bycode->index);
        break;
    }
    case 11: {
        CALL_BUILTIN(bycode->index);
        break;
    }
    case 12: {
        IMPORT();
        break;
    }
    case 13: {
        POW();
        break;
    }
    case 14: {
        ZDIV();
        break;
    }
    case 15: {
        MOD();
        break;
    }
    case 16: {
        IF_FALSE_GOTO(bycode->index);
        break;
    }
    case 17: {
        CHANGE_VALUE(bycode->index);
        break;
    }
    case 18: {
        EQUAL();
        break;
    }
    case 19: {
        UNEQUAL();
        break;
    }
    case 20: {
        GREATER_EQUAL();
        break;
    }
    case 21: {
        LESS_EQUAL();
        break;
    }
    case 22: {
        LESS();
        break;
    }
    case 23: {
        GREATER();
        break;
    }
    case 24: {
        ASSERT();
        break;
    }
    case 25: {
        NOT();
        break;
    }
    case 26: {
        AND();
        break;
    }
    case 27: {
        OR();
        break;
    }
    case 28: {
        STORE_LOCAL(bycode->index);
        break;
    }
    case 29: {
        LOAD_LOCAL(bycode->index);
        break;
    }
    case 30: {
        CALL_FUNCTION(bycode->index);
        break;
    }
    case 31: {
        FREE_FUNCTION();
        break;
    }
    case 32: {
        CHANGE_LOCAL(bycode->index);
        break;
    }
    case 33: {
        LOAD_LONG(bycode->index);
        break;
    }
    case 34: {
        LOAD_ARRAY(bycode->index);
        break;
    }
    case 35: {
        CALL_METHOD(bycode->index);
        break;
    }
    case 36: {
        LOAD_MAP(bycode->index);
        break;
    }
    }
}

void free_module(TVM* vm) {
    for (size_t i = 0; i < vm->modules_num; ++i) {
        if (vm->modules[i]->modules_num != 0) {
            free_module(vm->modules[i]);
        }
        delete vm->modules[i];
    }
}

TVM::~TVM() {
    // 释放字节码和变量的值
    free_module(this);
}

void TVM::pop_value() {
    --dyna_data.stack_top_ptr;
}

void TVM::run_all() {
    size_t n;
    for (run_index = 0, n = static_data.byte_codes.size(); run_index < n;
         ++run_index) {
        this->run_bycode(&static_data.byte_codes[run_index]);
    }
}

void TVM::run_func(const trc::TVM_space::func_& function) {
}

void TVM::error_report(int error, ...) {
    va_list ap;
    va_start(ap, error);
    error::send_error_module_aplist(
        error, name, static_data.line_number_table[run_index] + 1, ap);
    va_end(ap);
}

void TVM::run_line_bycode() {
    size_t now_index = static_data.line_number_table[run_index];
    do {
        run_one_bycode();
    } while (now_index != static_data.line_number_table.size() - 1
        && now_index
            == static_data.line_number_table
                   [run_index]); // 如果没有执行到底或者到下一行
}
}
