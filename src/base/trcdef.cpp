/**
 * 共享数据
 */
#include <string>
#include <map>
#include <array>
#include "base/trcdef.h"
#include "base/trcdef.h"
#include "base/dll.h"

using namespace std;

namespace trc {
    namespace def {
        // 虚拟机中的数字代号对应的类型
        const float version = 0.2;
    }

    namespace run_env {
        // 执行环境

        // 当前执行模块
        string run_module("__main__");
        // 当前执行到的模块行数
        map<string, int> lines;

        void set_module(const string &name) {
            /**
             * 设置当前编译模块(新建)
             * name:模块名
             */

            run_env::run_module = name;
            run_env::lines[name] = 0;
        }
    }
}
