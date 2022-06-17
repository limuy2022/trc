#include <TVM/TVM.h>
#include <TVM/func.h>
#include <TVMbase/TRE.h>
#include <TVMbase/types/string.h>
#include <base/Error.h>
#include <base/utils/data.hpp>
#include <language/error.h>
#include <string>

#define NAMEERROR(name)                                    \
    if (!utils::map_check_in_first(                        \
            dyna_data.var_names, name)) {                  \
        error_report(error::NameError,                     \
            language::error::modulenotfounderror, (name)); \
    }

namespace trc::TVM_space {
void TVM::CHANGE_VALUE(bytecode_index_t index) {
    const std::string& name_v
        = static_data.const_name[index];
    NAMEERROR(name_v.c_str());
    firstv = pop();
    dyna_data.var_names[name_v] = firstv;
}

void TVM::DEL() {
    firsti = (def::INTOBJ)pop();
    int n = firsti->value;
    for (int i = 0; i < n; ++i) {
        firsts = (def::STRINGOBJ)pop();
        NAMEERROR(firsts->c_str());
        dyna_data.var_names.erase(
            std::string(firsts->c_str()));
    }
}

void TVM::LOAD_NAME(bytecode_index_t index) {
    const std::string& name(static_data.const_name[index]);
    NAMEERROR(name.c_str());
    push(dyna_data.var_names[name]);
}

void TVM::STORE_NAME(bytecode_index_t index) {
    firstv = pop();
    dyna_data.var_names[static_data.const_name[index]]
        = firstv;
}

void TVM::STORE_LOCAL(bytecode_index_t name) {
    frame_* fast = dyna_data.frames.top();
    firstv = pop();
    fast->dyna_data.var_names[static_data.const_name[name]]
        = firstv;
}

void TVM::LOAD_LOCAL(bytecode_index_t name) {
    const std::string& s_name
        = static_data.const_name[name];
    NAMEERROR(s_name.c_str());
    push(dyna_data.frames.top()
             ->dyna_data.var_names[s_name]);
}

void TVM::CHANGE_LOCAL(bytecode_index_t index) {
    const std::string& name_v
        = static_data.const_name[index];
    NAMEERROR(name_v.c_str());
    firstv = pop();
    dyna_data.frames.top()->dyna_data.var_names[name_v]
        = firstv;
}

void TVM::DEL_LOCAL() {
    firsti = (def::INTOBJ)pop();
    frame_* fast = dyna_data.frames.top();
    int n = firsti->value;
    for (int i = 0; i < n; ++i) {
        firsts = (def::STRINGOBJ)pop();
        NAMEERROR(firsts->c_str());
        fast->dyna_data.var_names.erase(firsts->c_str());
    }
}
}
