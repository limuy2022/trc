#pragma once

#include "TVM/TVM.h"
#include "dll.h"

using namespace std;

namespace trc {
    namespace loader {
        TRC_base_api extern map<string, TVM_space::bytecode_t> codes_int;
        TRC_base_api extern string int_code[];
    }
}
