#include "TVM/TVM.h"
#include "TVM/built_in_func.h"
#include "base/trcdef.h"
#include "TVMbase/types/int.h"
#include "TVMbase/TRE.h"
#include "TVM/func.h"

using namespace std;

namespace trc
{
    namespace TVM_space
    {
        typedef void (*TVM_BUILTINS_FUNC)(int argc, TVM *vm);

        TVM_BUILTINS_FUNC builtin_funcs[]{
            &builtin::EXIT,
            &builtin::PRINT,
            &builtin::PRINTLN,
            &builtin::INPUT,
            &builtin::LENGTH,
            &builtin::HELP,
            &builtin::STRING_,
            &builtin::INT_,
            &builtin::BOOL_,
            &builtin::FLOAT_,
            &builtin::TYPE};

        void TVM::CALL_BUILTIN(const short &name)
        {
            /**
             * 执行内置函数
             */
            firsti = (def::INTOBJ)pop();
            builtin_funcs[name](firsti->value, this);
        }

        void TVM::CALL_FUNCTION(const short &index)
        {
            /**
             * 执行自定义函数
             * 事实上是在虚拟机内创建帧，在帧中完成字节码运行
             * 帧就相当于一个小环境
             */

            func_ *fast = static_data.funcs[static_data.const_name[index]];
            run_func_str = fast->name;
            frame_ *frame_var = new frame_;
            dyna_data.frames.push(frame_var);
            // 调用函数
            this->run_func(fast->bytecodes, LINE_NOW);
        }

        void TVM::FREE_FUNCTION()
        {
            /**
             * 还原到调用函数前
             */

            delete dyna_data.frames.top();
            dyna_data.frames.pop();
        }

        void TVM::CALL_METHOD(const short &index)
        {
        }
    }
}
