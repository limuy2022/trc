#pragma once

#include <string>
#include <map>
#include "TVMbase/TVM_data.h"
#include "TVMdef.h"

using namespace std;

namespace trc {
    namespace TVM_space {
        class frame_{
        public:
            TVM_dyna_data dyna_data;
            frame_();
        };

        class func_ {
        public:
            // 函数名
            const string name;

            // 字节码信息
            vector<vector<TVM_bytecode *> > bytecodes;
        };
    }
}
