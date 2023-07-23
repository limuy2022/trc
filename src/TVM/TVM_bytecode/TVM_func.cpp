module;
#include <format>
module TVM;
import unreach;
import built_in_func;
import func;
import trcdef;
import TVMdef;

namespace trc::TVM_space {
void TVM::CALL_BUILTIN(bytecode_index_t builtin_func_name) {
    auto firsti = (def::INTOBJ)pop();
    switch (builtin_func_name) {
    case 0: {
        builtin::EXIT(firsti->value, this);
        break;
    }
    case 1: {
        builtin::PRINT(firsti->value, this);
        break;
    }
    case 2: {
        builtin::PRINTLN(firsti->value, this);
        break;
    }
    case 3: {
        builtin::INPUT(firsti->value, this);
        break;
    }
    case 4: {
        builtin::LENGTH(firsti->value, this);
        break;
    }
    case 5: {
        builtin::HELP(firsti->value, this);
        break;
    }
    case 6: {
        builtin::STRING_(firsti->value, this);
        break;
    }
    case 7: {
        builtin::INT_(firsti->value, this);
        break;
    }
    case 8: {
        builtin::BOOL_(firsti->value, this);
        break;
    }
    case 9: {
        builtin::FLOAT_(firsti->value, this);
        break;
    }
    case 10: {
        builtin::TYPE(firsti->value, this);
        break;
    }
    default: {
        unreach(std::format("there is not a builin function {} to call",
            (short)builtin_func_name));
    }
    }
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
