#include "TVM/func.h"

namespace trc::TVM_space {
frame_::frame_() = default;

func_::~func_() {
    delete[] name;
}
}
