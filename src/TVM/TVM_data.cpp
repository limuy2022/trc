#include "TVMbase/TVM_data.h"

using namespace std;

namespace trc {
    namespace TVM_space {
        symbol_form::~symbol_form() {
            delete []vars;
        }
    }
}
