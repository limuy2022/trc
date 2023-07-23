module;
#include <stack>
#include <string>
#include <utility>
#include <format>
module TVM;
import TVM.memory;
import func;
import TVMdef;
import unreach;

namespace trc::TVM_space {
TVM::TVM(std::string name)
    : name(std::move(name)) {
    // 初始化TVM
    TVM_space::init_mem();
}

void TVM::reload_data() {
    dyna_data.reset_global_symbol_table(static_data.global_symbol_table_size);
}

void TVM::run_bycode(const TVM_bytecode* bycode) {
    switch (bycode->bycode) {
    case byteCodeNumber::LOAD_INT: {
        LOAD_INT(bycode->index);
        break;
    }
    case byteCodeNumber::ADD: {
        ADD();
        break;
    }
    case byteCodeNumber::SUB: {
        SUB();
        break;
    }
    case byteCodeNumber::MUL: {
        MUL();
        break;
    }
    case byteCodeNumber::DIV: {
        DIV();
        break;
    }
    case byteCodeNumber::GOTO: {
        GOTO(bycode->index);
        break;
    }
    case byteCodeNumber::STORE_NAME: {
        STORE_NAME(bycode->index);
        break;
    }
    case byteCodeNumber::LOAD_NAME: {
        LOAD_NAME(bycode->index);
        break;
    }
    case byteCodeNumber::LOAD_FLOAT: {
        LOAD_FLOAT(bycode->index);
        break;
    }
    case byteCodeNumber::LOAD_STRING: {
        LOAD_STRING(bycode->index);
        break;
    }
    case byteCodeNumber::CALL_BUILTIN: {
        CALL_BUILTIN(bycode->index);
        break;
    }
    case byteCodeNumber::IMPORT: {
        IMPORT();
        break;
    }
    case byteCodeNumber::POW: {
        POW();
        break;
    }
    case byteCodeNumber::ZDIV: {
        ZDIV();
        break;
    }
    case byteCodeNumber::MOD: {
        MOD();
        break;
    }
    case byteCodeNumber::IF_FALSE_GOTO: {
        IF_FALSE_GOTO(bycode->index);
        break;
    }
    case byteCodeNumber::CHANGE_VALUE: {
        CHANGE_VALUE(bycode->index);
        break;
    }
    case byteCodeNumber::EQUAL: {
        EQUAL();
        break;
    }
    case byteCodeNumber::UNEQUAL: {
        UNEQUAL();
        break;
    }
    case byteCodeNumber::GREATER_EQUAL: {
        GREATER_EQUAL();
        break;
    }
    case byteCodeNumber::LESS_EQUAL: {
        LESS_EQUAL();
        break;
    }
    case byteCodeNumber::LESS: {
        LESS();
        break;
    }
    case byteCodeNumber::GREATER: {
        GREATER();
        break;
    }
    case byteCodeNumber::ASSERT: {
        ASSERT();
        break;
    }
    case byteCodeNumber::NOT: {
        NOT();
        break;
    }
    case byteCodeNumber::AND: {
        AND();
        break;
    }
    case byteCodeNumber::OR: {
        OR();
        break;
    }
    case byteCodeNumber::STORE_LOCAL: {
        STORE_LOCAL(bycode->index);
        break;
    }
    case byteCodeNumber::LOAD_LOCAL: {
        LOAD_LOCAL(bycode->index);
        break;
    }
    case byteCodeNumber::CALL_FUNCTION: {
        CALL_FUNCTION(bycode->index);
        break;
    }
    case byteCodeNumber::FREE_FUNCTION: {
        FREE_FUNCTION();
        break;
    }
    case byteCodeNumber::CHANGE_LOCAL: {
        CHANGE_LOCAL(bycode->index);
        break;
    }
    case byteCodeNumber::LOAD_LONG: {
        LOAD_LONG(bycode->index);
        break;
    }
    case byteCodeNumber::LOAD_ARRAY: {
        LOAD_ARRAY(bycode->index);
        break;
    }
    case byteCodeNumber::CALL_METHOD: {
        CALL_METHOD(bycode->index);
        break;
    }
    case byteCodeNumber::LOAD_MAP: {
        LOAD_MAP(bycode->index);
        break;
    }
    default: {
        unreach(std::format("Unsupport bytecode!{}", int(bycode->bycode)));
    }
    }
}

void free_module(TVM* vm) {
    for (size_t i = 0; i < vm->modules_num; ++i) {
        free_module(vm->modules[i]);
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
        run_bycode(&static_data.byte_codes[run_index]);
    }
}

void TVM::run_func(const trc::TVM_space::func_& function) {
    // 暂存代码索引
    size_t save_now = run_index, n;
    for (run_index = 0, n = function.bytecodes.size(); run_index < n;
         ++run_index) {
        run_bycode(&function.bytecodes[run_index]);
    }
    run_index = save_now;
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
