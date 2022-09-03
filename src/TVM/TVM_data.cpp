#include <TVM/TVM_data.h>
#include <cstdlib>

namespace trc::TVM_space {
void TVM_dyna_data::reset_global_symbol_table(size_t size) {
    global_symbol_table.reset(size);
}

TVM_dyna_data::TVM_dyna_data()
    : stack_top_ptr(stack_data - 1)
    , frames_top_str(frames - 1) {
}

void TVM_static_data::ReleaseStringData() {
    for (size_t i = 0; i < const_s.size; ++i) {
        free(const_s.array[i]);
    }
    const_s.clear();
    for (size_t i = 0; i < const_long.size; ++i) {
        free(const_long.array[i]);
    }
    const_long.clear();
}

TVM_static_data::~TVM_static_data() {
    ReleaseStringData();
}

void TVM_static_data::compress_memory() {
    const_i.compress_memory();
    const_s.compress_memory();
    const_long.compress_memory();
    const_f.compress_memory();
}
}
