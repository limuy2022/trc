#include <TVM/TVM_data.hpp>
#include <cstdlib>

namespace trc::TVM_space {
void TVM_dyna_data::reset_global_symbol_table(size_t size) {
    global_symbol_table.reset(size);
}

TVM_dyna_data::TVM_dyna_data()
    : stack_top_ptr(stack_data - 1)
    , frames_top_str(frames - 1) {
}

void TVM_static_data::compress_memory() {
    const_i.shrink_to_fit();
    const_s.shrink_to_fit();
    const_long.shrink_to_fit();
    const_f.shrink_to_fit();
    funcs.shrink_to_fit();
}
}
