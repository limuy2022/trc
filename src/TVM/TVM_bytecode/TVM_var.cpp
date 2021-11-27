#include <string>
#include "TVM/TVM.h"
#include "base/utils/data.hpp"
#include "base/Error.h"
#include "TVMbase/types/string.h"
#include "TVMbase/TRE.h"
#include "TVM/func.h"

using namespace std;

#define NAMEERROR(name)                                        \
    if (!utils::map_check_in_first(dyna_data.var_names, name)) \
    {                                                          \
        error::send_error(error::NameError, (name));           \
    }

namespace trc
{
    namespace TVM_space
    {
        void TVM::CHANGE_VALUE(const short &index)
        {
            /**
             * 改变变量的值
             */

            const string &name_v = static_data.const_name[index];
            NAMEERROR(name_v.c_str());
            firstv = pop();
            dyna_data.var_names[name_v] = firstv;
        }

        void TVM::DEL()
        {
            /**
             * 删除变量
             */

            firsti = (def::INTOBJ)pop();
            int n = firsti->value;
            for (int i = 0; i < n; ++i)
            {
                firsts = (def::STRINGOBJ)pop();
                NAMEERROR(firsts->c_str());
                dyna_data.var_names.erase(string(firsts->c_str()));
            }
        }

        void TVM::LOAD_NAME(const short &index)
        {
            /**
             * 读取变量的值
             */
            const string &name(static_data.const_name[index]);
            NAMEERROR(name.c_str());
            push(dyna_data.var_names[name]);
        }

        void TVM::STORE_NAME(const short &index)
        {
            /**
             * 新建变量
             */
            firstv = pop();
            dyna_data.var_names[static_data.const_name[index]] = firstv;
        }

        void TVM::STORE_LOCAL(const short &name)
        {
            /**
             * 生成函数局部变量
             */
            frame_ *fast = dyna_data.frames.top();
            firstv = pop();
            fast->dyna_data.var_names[static_data.const_name[name]] = firstv;
        }

        void TVM::LOAD_LOCAL(const short &name)
        {
            /**
             * 读取函数局部变量
             */
            const string &s_name = static_data.const_name[name];
            NAMEERROR(s_name.c_str());
            push(dyna_data.frames.top()->dyna_data.var_names[s_name]);
        }

        void TVM::CHANGE_LOCAL(const short &index)
        {
            /**
             * 改变局部变量的值
             */

            const string &name_v = static_data.const_name[index];
            NAMEERROR(name_v.c_str());
            firstv = pop();
            dyna_data.frames.top()->dyna_data.var_names[name_v] = firstv;
        }

        void TVM::DEL_LOCAL()
        {
            /**
             * 删除局部变量
             */
            firsti = (def::INTOBJ)pop();
            frame_ *fast = dyna_data.frames.top();
            int n = firsti->value;
            for (int i = 0; i < n; ++i)
            {
                firsts = (def::STRINGOBJ)pop();
                NAMEERROR(firsts->c_str());
                fast->dyna_data.var_names.erase(firsts->c_str());
            }
        }
    }
}
