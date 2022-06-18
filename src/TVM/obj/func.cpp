#include <TVM/func.h>

namespace trc::TVM_space {
frame_::frame_(func_* func)
    : dyna_data(func->symbol_form_size) {
}

func_::~func_() {
    delete[] name;
}
}
