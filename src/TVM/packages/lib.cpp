#include "TVM/lib.h"

using namespace std;

cpp_lib::cpp_lib(int funcnums) :
        funcs_nums(funcnums),
        funcs(new char *[funcnums]) {};

cpp_lib::~cpp_lib() {
    for (int i = 0; i < funcs_nums; ++i) {
        delete[]funcs[i];
    }
    delete[]funcs;
}

namespace cpp_libs {
    size_t libs_num = 1;

    string names[] = {
            "math"
    };
}
