#include <TVM/func.h>

namespace trc::TVM_space {
func_::func_(char* name)
    : name(name) {
}

void frame_::set_func(const trc::TVM_space::func_& func) {
}

void frame_::free_func() {
}

func_::~func_() {
    delete[] name;
}
}
