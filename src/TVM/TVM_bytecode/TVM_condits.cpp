#include "TVM/TVM.h"
#include "base/Error.h"
#include "TVMbase/TRE.h"
#include "base/trcdef.h"
#include "TVMbase/types/string.h"

using namespace std;

namespace trc
{
    namespace TVM_space
    {
        void TVM::EQUAL()
        {
            /**
             * 判断相等，假设两端类型相等
             * 注意：如果不相等，将会在编译期间进行强制转换，不能通过者将会报出类型错误
             */

            secondv = pop();
            firstv = pop();
            push(firstv->operator==(secondv));
        }

        void TVM::UNEQUAL()
        {
            /**
             * 判断不相等
             */
            secondv = pop();
            firstv = pop();
            push(firstv->operator!=(secondv));
        }

        void TVM::GREATER_EQUAL()
        {
            /**
             * 判断大于等于
             */
            secondv = pop();
            firstv = pop();
            push(firstv->operator>=(secondv));
        }

        void TVM::LESS_EQUAL()
        {
            /**
             * 判断小于等于
             */
            secondv = pop();
            firstv = pop();
            push(firstv->operator<=(secondv));
        }

        void TVM::LESS()
        {
            /**
             * 判断小于
             */

            secondv = pop();
            firstv = pop();
            push(firstv->operator<(secondv));
        }

        void TVM::GREATER()
        {
            /**
             * 判断大于
             */
            secondv = pop();
            firstv = pop();
            push(firstv->operator>(secondv));
        }

        void TVM::IF_FALSE_GOTO(const short &index)
        {
            /**
             * 如果为否，跳转代码
             */

            firsti = (def::INTOBJ)(pop());
            if (!firsti->value)
                GOTO(index);
        }

        void TVM::ASSERT()
        {
            /**
             * 断言，判断表达式是否为假
             */

            firsti = (def::INTOBJ)pop();
            secondi = (def::INTOBJ)pop();
            int tmp_i = firsti->value;
            if (!secondi->value)
            {
                if (tmp_i > 2)
                    error::send_error(error::ArgumentError, "assert needs one or two arguments.");

                switch (tmp_i)
                {
                case 1:
                    error::send_error(error::AssertError, "assert.");
                    break;
                case 2:
                    firsts = (def::STRINGOBJ)pop();
                    error::send_error(error::AssertError, firsts->c_str());
                    break;
                }
            }
        }

        void TVM::NOT()
        {
            firstv = pop();
            push(firstv->operator!());
        }

        void TVM::AND()
        {
            secondv = pop();
            firstv = pop();
            push(firstv->operator&&(secondv));
        }

        void TVM::OR()
        {
            secondv = pop();
            firstv = pop();
            push(firstv->operator||(secondv));
        }
    }
}
