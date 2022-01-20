#include "TVM/lib.h"
#include <array>

namespace trc::TVM_space::cpp_libs {
std::array<std::string, 1> names = { "math" };

cpp_lib::cpp_lib(int funcnums)
    : funcs_nums(funcnums)
    , funcs(new char*[funcnums]) {
}

cpp_lib::~cpp_lib() {
    for (int i = 0; i < funcs_nums; ++i) {
        delete[] funcs[i];
    }
    delete[] funcs;
}
}
