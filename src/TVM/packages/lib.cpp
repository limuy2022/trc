#include <array>
#include "TVM/lib.h"

using namespace std;

namespace trc {
    namespace TVM_space {
        namespace cpp_libs {
            array<string, 1> names = {
                    "math"
            };

            cpp_lib::cpp_lib(int funcnums) :
                    funcs_nums(funcnums),
                    funcs(new char *[funcnums]) {}

            cpp_lib::~cpp_lib() {
                for (int i = 0; i < funcs_nums; ++i) {
                    delete[]funcs[i];
                }
                delete[]funcs;
            }
        }
    }
}
