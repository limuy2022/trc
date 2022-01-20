#include "TVMbase/TVM_data.h"

namespace trc::TVM_space {
symbol_form::~symbol_form() {
    delete[] vars;
}

void TVM_static_data::ReleaseStringData() {
    for (auto i : const_s) {
        delete[] i;
    }
    for (auto i : const_name) {
        delete[] i;
    }
    for (auto i : const_long) {
        delete[] i;
    }
}

TVM_static_data::~TVM_static_data() {
    ReleaseStringData();
}
}
