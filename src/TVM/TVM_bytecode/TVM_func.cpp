#include <TVM/TVM.h>
#include <TVM/built_in_func.h>
#include <TVM/func.h>
#include <TVMbase/TRE.h>
#include <base/trcdef.h>

namespace trc::TVM_space {
TVM_BUILTINS_FUNC builtin_funcs[] { &builtin::EXIT, &builtin::PRINT,
    &builtin::PRINTLN, &builtin::INPUT, &builtin::LENGTH, &builtin::HELP,
    &builtin::STRING_, &builtin::INT_, &builtin::BOOL_, &builtin::FLOAT_,
    &builtin::TYPE };

void TVM::CALL_BUILTIN(bytecode_index_t name) {
    firsti = (def::INTOBJ)pop();
    builtin_funcs[name](firsti->value, this);
}

void TVM::CALL_FUNCTION(bytecode_index_t index) {
    func_* fast = static_data.funcs[static_data.const_name[index]];
    run_func_str = fast->name;
    frame_* frame_var = new frame_;
    dyna_data.frames.push(frame_var);
    // 调用函数
    // this->run_func(fast->bytecodes, LINE_NOW);
}

void TVM::FREE_FUNCTION() {
    delete dyna_data.frames.top();
    dyna_data.frames.pop();
}

void TVM::CALL_METHOD(bytecode_index_t index) {
}
}
