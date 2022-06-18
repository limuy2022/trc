#include <TVM/memory.h>
#include <TVM/TVM.h>
#include <TVM/func.h>
#include <base/Error.h>
#include <base/memory/memory.h>
#include <base/trcdef.h>
#include <cstdarg>
#include <utility>

namespace trc::TVM_space {

// 利用索引存放函数指针，实现O(1)复杂度的调用算法
// 不符合的地方用nullptr代替，算是以小部分空间换大部分时间
const NOARGV_TVM_METHOD TVM::TVM_RUN_CODE_NOARG_FUNC[]
    = { nullptr, &trc::TVM_space::TVM::ADD, &trc::TVM_space::TVM::NOP,
          &trc::TVM_space::TVM::SUB, &trc::TVM_space::TVM::MUL,
          &trc::TVM_space::TVM::DIV, nullptr, nullptr, nullptr,
          &trc::TVM_space::TVM::DEL, nullptr, nullptr, nullptr,
          &trc::TVM_space::TVM::IMPORT, &trc::TVM_space::TVM::POW,
          &trc::TVM_space::TVM::ZDIV, &trc::TVM_space::TVM::MOD, nullptr,
          nullptr, &trc::TVM_space::TVM::EQUAL, &trc::TVM_space::TVM::UNEQUAL,
          &trc::TVM_space::TVM::GREATER_EQUAL, &trc::TVM_space::TVM::LESS_EQUAL,
          &trc::TVM_space::TVM::LESS, &trc::TVM_space::TVM::GREATER,
          &trc::TVM_space::TVM::ASSERT, &trc::TVM_space::TVM::NOT,
          &trc::TVM_space::TVM::AND, &trc::TVM_space::TVM::OR, nullptr, nullptr,
          nullptr, &trc::TVM_space::TVM::FREE_FUNCTION, nullptr,
          &trc::TVM_space::TVM::DEL_LOCAL, nullptr, nullptr, nullptr, nullptr };

const ARGV_TVM_METHOD TVM::TVM_RUN_CODE_ARG_FUNC[]
    = { &trc::TVM_space::TVM::LOAD_INT, nullptr, nullptr, nullptr, nullptr,
          nullptr, &trc::TVM_space::TVM::GOTO, &trc::TVM_space::TVM::STORE_NAME,
          &trc::TVM_space::TVM::LOAD_NAME, nullptr,
          &trc::TVM_space::TVM::LOAD_FLOAT, &trc::TVM_space::TVM::LOAD_STRING,
          &trc::TVM_space::TVM::CALL_BUILTIN, nullptr, nullptr, nullptr,
          nullptr, &trc::TVM_space::TVM::IF_FALSE_GOTO,
          &trc::TVM_space::TVM::CHANGE_VALUE, nullptr, nullptr, nullptr,
          nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
          &trc::TVM_space::TVM::STORE_LOCAL, &trc::TVM_space::TVM::LOAD_LOCAL,
          &trc::TVM_space::TVM::CALL_FUNCTION, nullptr,
          &trc::TVM_space::TVM::CHANGE_LOCAL, nullptr,
          &trc::TVM_space::TVM::LOAD_LONG, &trc::TVM_space::TVM::LOAD_ARRAY,
          &trc::TVM_space::TVM::CALL_METHOD, &trc::TVM_space::TVM::LOAD_MAP };

TVM::TVM(std::string name)
    : name(std::move(name)) {
    // 初始化TVM
    TVM_space::init_mem();
    dyna_data.var_names["__name__"] = new types::trc_string("__main__");
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
    free_TVM(this);
    free_module(this);
    for (const auto& i : static_data.funcs) {
        delete i.second;
    }
}

void TVM::push(def::OBJ a) {
    dyna_data.stack_data.push(a);
}

void TVM::pop_value() {
    dyna_data.stack_data.pop();
}

def::OBJ TVM::pop() {
    def::OBJ a = dyna_data.stack_data.top();
    dyna_data.stack_data.pop();
    return a;
}

void TVM::run_all() {
    size_t n;
    for (run_index = 0, n = static_data.byte_codes.size(); run_index < n;
         ++run_index) {
        this->run_bycode(&static_data.byte_codes[run_index]);
    }
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
                   [run_index]); //如果没有执行到底或者到下一行
}

TVM* create_TVM(const std::string& name) {
    return new TVM(name);
}
}
