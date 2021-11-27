/**
 * 项目配置文件，包括大量基本类型定义
 * 注意：由于大量源文件都引用了本文件，所以尽量不要修改此文件，否则可能会造成编译时间过长
 */

#pragma once

#include <vector>
#include <string>
#include <map>
#include <array>
#include "dll.h"

// 当前模块行号
#define LINE_NOW run_env::lines[run_env::run_module]

using namespace std;

typedef vector<string> vecs;

namespace trc {
    namespace TVM_space {
        namespace types {
            class trc_long;

            class trcobj;
            
            class trc_int;
            
            class trc_float;
            
            class trc_string;
            
            class trc_flong;
        }
    }
    namespace def {
        typedef TVM_space::types::trcobj *OBJ;
        typedef TVM_space::types::trc_int *INTOBJ;
        typedef TVM_space::types::trc_float *FLOATOBJ;
        typedef TVM_space::types::trc_string *STRINGOBJ;
        typedef TVM_space::types::trc_long *LONGOBJ;
        typedef TVM_space::types::trc_flong *FLONGOBJ;

        typedef unsigned char byte_t;
        TRC_base_api extern const float version;
    }
    
    namespace run_env {
        // 执行环境

        TRC_base_api extern string run_module;
        // 当前执行到的模块函数
        TRC_base_api extern map<string, int> lines;

        TRC_base_api void set_module(const std::string &name);
    }
}
