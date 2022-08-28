#include <TVM/TVM.h>
#include <TVM/built_in_func.h>
#include <TVM/func.h>
#include <base/trcdef.h>

namespace trc::TVM_space {
TVM_BUILTINS_FUNC builtin_funcs[] { &builtin::EXIT, &builtin::PRINT,
    &builtin::PRINTLN, &builtin::INPUT, &builtin::LENGTH, &builtin::HELP,
    &builtin::STRING_, &builtin::INT_, &builtin::BOOL_, &builtin::FLOAT_,
    &builtin::TYPE };

void TVM::CALL_BUILTIN(bytecode_index_t name) {
    auto firsti = (def::INTOBJ)pop();
    builtin_funcs[name](firsti->value, this);
}

void TVM::CALL_FUNCTION(bytecode_index_t index) {
    const func_& fast = static_data.funcs[index];
    // 启动函数环境
    (++dyna_data.frames_top_str)->set_func(fast);
    // 调用函数
    this->run_func(fast);
}

void TVM::FREE_FUNCTION() {
    dyna_data.frames_top_str->free_func();
    dyna_data.frames_top_str--;
}

void TVM::CALL_METHOD(bytecode_index_t index) {
}
}
