/**
 * 虚拟机，执行TVM字节码的地方
 * 这里负责存放它的接口
 */

#pragma once

#include <string>
#include <vector>
#include <stack>
#include "TVMbase/TVM_data.h"
#include "base/utils/bytes.h"
#include "TVM/dll.h"

#define LOAD_INT_ "LOAD_INT"
#define ADD_ "ADD"
#define NOP_ "NOP"
#define SUB_ "SUB"
#define MUL_ "MUL"
#define DIV_ "DIV"
#define GOTO_ "GOTO"
#define STORE_NAME_ "STORE_NAME"
#define LOAD_NAME_ "LOAD_NAME"
#define DEL_ "DEL"
#define LOAD_FLOAT_ "LOAD_FLOAT"
#define LOAD_STRING_ "LOAD_STRING"
#define CALL_BUILTIN_ "CALL_BUILTIN"
#define IMPORT_ "IMPORT"
#define POW_ "POW"
#define ZDIV_ "ZDIV"
#define MOD_ "MOD"
#define IF_FALSE_GOTO_ "IF_FALSE_GOTO"
#define CHANGE_VALUE_ "CHANGE_VALUE"
#define EQUAL_ "EQUAL"
#define UNEQUAL_ "UNEQUAL"
#define GREATER_EQUAL_ "GREATER_EQUAL"
#define LESS_EQUAL_ "LESS_EQUAL"
#define LESS_ "LESS"
#define GREATER_ "GREATER"
#define ASSERT_ "ASSERT"
#define NOT_ "NOT"
#define AND_ "AND"
#define OR_ "OR"
#define STORE_LOCAL_ "STORE_LOCAL"
#define CALL_FUNCTION_ "CALL_FUNCTION"
#define FREE_FUNCTION_ "FREE_FUNCTION"
#define LOAD_LOCAL_ "LOAD_LOCAL"
#define CHANGE_LOCAL_ "CHANGE_LOCAL"
#define DEL_LOCAL_ "DEL_LOCAL"
#define LOAD_LONG_ "LOAD_LONG"
#define LOAD_ARRAY_ "LOAD_ARRAY"
#define CALL_METHOD_ "CALL_METHOD"
#define LOAD_MAP_ "LOAD_MAP"

using namespace std;

namespace trc
{
    namespace TVM_space
    {
        class TRC_TVM_api TVM
        {
            /**
             * TVM：trc的核心部分，负责执行字节码
             */

        public:
            TVM(const string &name, float ver_in = def::version);

            ~TVM();

            // 模块名
            string name;
            // 运行函数模块名
            string run_func_str;
            // 调用函数的字节码所在的行
            int call_func_index;

            map<string, TVM *> modules;

            void run();

            void run_func(const struct_codes &bytecodes_, int init_index);

            void run_step();

            def::OBJ pop();

            void pop_value();

            void push(def::OBJ a);

            /* 指令集开始定义处 */
            void LOAD_INT(const short &index);

            void LOAD_FLOAT(const short &index);

            void LOAD_STRING(const short &index);

            void ADD();

            void SUB();

            void MUL();

            void DIV();

            void ZDIV();

            void POW();

            void MOD();

            void NOP();

            void GOTO(const short &index);

            void STORE_NAME(const short &index);

            void LOAD_NAME(const short &index);

            void DEL();

            void CALL_BUILTIN(const short &name);

            void IMPORT();

            void IF_FALSE_GOTO(const short &index);

            void CHANGE_VALUE(const short &index);

            void EQUAL();

            void UNEQUAL();

            void GREATER_EQUAL();

            void LESS_EQUAL();

            void LESS();

            void GREATER();

            void NOT();

            void AND();

            void OR();

            void ASSERT();

            void STORE_LOCAL(const short &name);

            void LOAD_LOCAL(const short &name);

            void CALL_FUNCTION(const short &index);

            void FREE_FUNCTION();

            void CHANGE_LOCAL(const short &index);

            void DEL_LOCAL();

            void LOAD_LONG(const short &index);

            void CALL_METHOD(const short &index);

            void LOAD_ARRAY(const short &index);

            void LOAD_MAP(const short &index);

            /* 指令集结束定义处 */

            // 静态数据，编译时生成
            TVM_data static_data;

            TVM_dyna_data dyna_data;
        };

        TRC_TVM_api TVM *create_TVM(const string &name = "__main__");
    }
}
