/**
 * TVM各种数据集合的定义处
 */

#pragma once

#include <string>
#include <vector>
#include <stack>
#include <map>
#include "TVM/TVMdef.h"
#include "TVM/object.h"

using namespace std;

namespace trc {
    namespace TVM_space {
        class func_;
        class frame_;

        struct TVM_bytecode {
            bytecode_t bycode;
            index_t index;
        };

        struct TVM_data {
            /**
             * 静态数据
             * 本类用于分离TVM的职责，装载编译时的数据，如常量池等
             * 并且降低TVM和Compiler之间的耦合度
             */
            // 各种常量数据
            vector<int> const_i;
            vecs const_s, const_name, const_long;
            vector<double> const_f;
            // 注意：此处装载func的静态信息，并不是保存执行信息的场所
            map<string, func_ *> funcs;
            // 版本号
            float ver_;
            // 字节码
            struct_codes byte_codes;
            // 类信息
            vector<class_header> class_msg;
        };

        class TVM_dyna_data {
            /**
             * TVM运行过程中动态数据的存放地
             * 派生类：例如：TVM， frame_
             */
        public:
            // 变量
            map<string, def::OBJ> var_names;
            // 操作数栈
            stack<def::OBJ> stack_data;
            // 帧栈
            stack<frame_ *> frames;
        };

        class symbol_form {
            /**
             * 符号表，保存变量信息
             * 就是凭借这个结构优化变量访问速度为O(1)
             */
        public:
            int *vars;

            ~symbol_form();
        };
    }
}
