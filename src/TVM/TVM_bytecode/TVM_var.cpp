#include <TVM/TVM.h>
#include <TVM/types/trc_string.h>

namespace trc::TVM_space {
void TVM::CHANGE_VALUE(bytecode_index_t index) {
    dyna_data.global_symbol_table.vars[index] = pop();
}

void TVM::LOAD_NAME(bytecode_index_t index) {
    push(dyna_data.global_symbol_table.vars[index]);
}

void TVM::STORE_NAME(bytecode_index_t index) {
    dyna_data.global_symbol_table.vars[index] = pop();
}

void TVM::STORE_LOCAL(bytecode_index_t var_index) {
    dyna_data.frames_top_str->localvars.vars[var_index] = pop();
}

void TVM::LOAD_LOCAL(bytecode_index_t var_index) {
    push(dyna_data.frames_top_str->localvars.vars[var_index]);
}

void TVM::CHANGE_LOCAL(bytecode_index_t index) {
    dyna_data.frames_top_str->localvars.vars[index] = pop();
}
}
