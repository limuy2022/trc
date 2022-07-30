#include <TVM/TRE.h>
#include <TVM/TVM.h>
#include <TVM/func.h>
#include <TVM/types/trc_string.h>
#include <base/utils/data.hpp>
#include <language/error.h>

namespace trc::TVM_space {
void TVM::CHANGE_VALUE(bytecode_index_t index) {
    dyna_data.global_symbol_table.vars[index] = pop();
}

void TVM::DEL() {
}

void TVM::LOAD_NAME(bytecode_index_t index) {
    push(dyna_data.global_symbol_table.vars[index]);
}

void TVM::STORE_NAME(bytecode_index_t index) {
    dyna_data.global_symbol_table.vars[index] = pop();
}

void TVM::STORE_LOCAL(bytecode_index_t name) {
    // frame_* fast = dyna_data.frames.top();
    // firstv = pop();
    // fast->dyna_data.var_names[static_data.const_name[name]] = firstv;
}

void TVM::LOAD_LOCAL(bytecode_index_t name) {
    // const std::string& s_name = static_data.const_name[name];
    // NAMEERROR(s_name.c_str());
    // push(dyna_data.frames.top()->dyna_data.var_names[s_name]);
}

void TVM::CHANGE_LOCAL(bytecode_index_t index) {
    // const std::string& name_v = static_data.const_name[index];
    // NAMEERROR(name_v.c_str());
    // firstv = pop();
    // dyna_data.frames.top()->dyna_data.var_names[name_v] = firstv;
}

void TVM::DEL_LOCAL() {
}
}
