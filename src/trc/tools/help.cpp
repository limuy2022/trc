#include <iostream>
#include "language/language.h"

using namespace std;

namespace trc {
    namespace tools_out {
        void help() {
            /**
             * Trc命令行操作帮助文档，因为代码简单所以放在主文件
             */

            cout << language::help::help_msg;
        }
    }
}
