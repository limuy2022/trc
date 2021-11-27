#include "base/Error.h"
#include "base/utils/type.hpp"
#include "TVMbase/TRE.h"
#include "TVM/TVM.h"
#include "TVMbase/types/string.h"
#include "base/trcdef.h"
#include "base/memory/memory.h"
#include "TVMbase/memory.h"

using namespace std;

// 利用索引存放函数指针，实现O(1)复杂度的调用算法
// 不符合的地方用nullptr代替，算是以小部分空间换大部分时间
const static trc::TVM_space::NOARGV_TVM_METHOD TVM_RUN_CODE_NOARG_FUNC[] = {
    nullptr,
    &trc::TVM_space::TVM::ADD,
    &trc::TVM_space::TVM::NOP,
    &trc::TVM_space::TVM::SUB,
    &trc::TVM_space::TVM::MUL,
    &trc::TVM_space::TVM::DIV,
    nullptr,
    nullptr,
    nullptr,
    &trc::TVM_space::TVM::DEL,
    nullptr,
    nullptr,
    nullptr,
    &trc::TVM_space::TVM::IMPORT,
    &trc::TVM_space::TVM::POW,
    &trc::TVM_space::TVM::ZDIV,
    &trc::TVM_space::TVM::MOD,
    nullptr,
    nullptr,
    &trc::TVM_space::TVM::EQUAL,
    &trc::TVM_space::TVM::UNEQUAL,
    &trc::TVM_space::TVM::GREATER_EQUAL,
    &trc::TVM_space::TVM::LESS_EQUAL,
    &trc::TVM_space::TVM::LESS,
    &trc::TVM_space::TVM::GREATER,
    &trc::TVM_space::TVM::ASSERT,
    &trc::TVM_space::TVM::NOT,
    &trc::TVM_space::TVM::AND,
    &trc::TVM_space::TVM::OR,
    nullptr,
    nullptr,
    nullptr,
    &trc::TVM_space::TVM::FREE_FUNCTION,
    nullptr,
    &trc::TVM_space::TVM::DEL_LOCAL,
    nullptr,
    nullptr,
    nullptr,
    nullptr};

const static trc::TVM_space::ARGV_TVM_METHOD TVM_RUN_CODE_ARG_FUNC[] = {
    &trc::TVM_space::TVM::LOAD_INT,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    &trc::TVM_space::TVM::GOTO,
    &trc::TVM_space::TVM::STORE_NAME,
    &trc::TVM_space::TVM::LOAD_NAME,
    nullptr,
    &trc::TVM_space::TVM::LOAD_FLOAT,
    &trc::TVM_space::TVM::LOAD_STRING,
    &trc::TVM_space::TVM::CALL_BUILTIN,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    &trc::TVM_space::TVM::IF_FALSE_GOTO,
    &trc::TVM_space::TVM::CHANGE_VALUE,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    &trc::TVM_space::TVM::STORE_LOCAL,
    &trc::TVM_space::TVM::LOAD_LOCAL,
    &trc::TVM_space::TVM::CALL_FUNCTION,
    nullptr,
    &trc::TVM_space::TVM::CHANGE_LOCAL,
    nullptr,
    &trc::TVM_space::TVM::LOAD_LONG,
    &trc::TVM_space::TVM::LOAD_ARRAY,
    &trc::TVM_space::TVM::CALL_METHOD,
    &trc::TVM_space::TVM::LOAD_MAP};

namespace trc
{
    namespace TVM_space
    {
        TVM::TVM(const string &name, float ver_in) : name(name)
        {
            static_data.ver_ = ver_in;
            // 新建默认变量
            dyna_data.var_names["__name__"] = new types::trc_string("__main__");
        }

        TVM::~TVM()
        {
            // 释放字节码和变量的值
            free_TVM(this);
            // 重置
            run_env::lines.erase(name);
            for (const auto &i : static_data.funcs)
            {
                delete i.second;
            }
        }

        void TVM::run()
        {
            /**
             * 从头开始，执行所有字节码
             */

            string &parent = run_env::run_module;
            run_env::run_module = name;

            // 将整个程序当成一个函数执行
            this->run_func(static_data.byte_codes, 0);

            run_env::run_module = parent;
        }

        void TVM::run_func(const struct_codes &bytecodes_, int init_index)
        {
            /**
             * 执行局部字节码（函数）
             * init_index:代码运行初始化索引
             */

            // 原先的代码行数，用于还原索引
            int re_ = LINE_NOW;
            LINE_NOW = init_index;
            auto *pointer = &LINE_NOW;
            size_t size = bytecodes_.size();
            while (*pointer < size)
            {
                for (const auto &i : bytecodes_[*pointer])
                    if (i->index == -1)
                        (this->*TVM_RUN_CODE_NOARG_FUNC[i->bycode])();
                    else
                        (this->*TVM_RUN_CODE_ARG_FUNC[i->bycode])(i->index);
                (*pointer)++;
            }
            *pointer = re_;
        }

        void TVM::run_step()
        {
            /**
             * 单步执行，为tdb调试功能
             * 注意：此功能和run函数不能混用！
             */

            auto *pointer = &LINE_NOW;
            for (const auto &i : static_data.byte_codes[*pointer])
                if (i->index == -1)
                    (this->*TVM_RUN_CODE_NOARG_FUNC[i->bycode])();
                else
                    (this->*TVM_RUN_CODE_ARG_FUNC[i->bycode])(i->index);
            (*pointer)++;
        }

        void TVM::push(def::OBJ a)
        {
            dyna_data.stack_data.push(a);
        }

        void TVM::pop_value()
        {
            /**
             * 弹出栈顶的值
             * 注意：作为一个不可能被利用的值，在这里会被直接析构
             */
            delete dyna_data.stack_data.top();
            dyna_data.stack_data.pop();
        }

        def::OBJ TVM_space::TVM::pop()
        {
            /**
             * 弹出栈顶的数据指针
             */
            def::OBJ a = dyna_data.stack_data.top();
            dyna_data.stack_data.pop();
            return a;
        }

        TVM *create_TVM(const string &name)
        {
            run_env::set_module(name);
            return new TVM(name);
        }

        void free_module(map<string, TVM_space::TVM *> &modules)
        {
            /**
             * 递归删除TVM模块内存
             */
            for (const auto &i : modules)
                if (i.second->modules.empty())
                    delete i.second;
                else
                    free_module(i.second->modules);
        }
    }
}
