#include <iostream>
#include <string>
#include <cstdarg>
#include <stack>
#include "base/dll.h"
#include "language/language.h"
#include "base/Error.h"
#include "base/trcdef.h"

using namespace std;

namespace trc {
    namespace error {
        typedef string error_t;
        
        namespace error_env {
            /**
             * 报错设置
             * 系统需要知道当前处于什么模式，以合适的模式应对发生的状况
             */
            // 是否终止程序
            bool quit = true;
            // 是否显示出错误信息，如果为否将会把错误信息存入栈中
            bool out_msg = true;
            // 异常栈
            stack<error_t> error_stack;
        }

        string make_error_msg(error_type error_name, va_list &ap) {
            /**
             * 通用函数，填充报错信息
             * error_name：异常名
             * ap：可变参数
             */

            string finally_out = language::error::error_map[error_name];
            string::size_type index;
            for (;;) {
                index = finally_out.find('$');
                if (index == string::npos)
                    return finally_out;
                finally_out.replace(index, 1, va_arg(ap, const char*));
            }
        }

        void send_error(error_type name, ...) {
            /**
             * 运行时报出错误，错误名称和错误信息
             * name:错误名
             * 可变参数：const char* 类型的字符串
             */

            va_list ap;
            va_start(ap, name);
            send_error_(make_error_msg(name, ap));
            va_end(ap);
            if (error_env::quit) exit(1);
        }

        void send_error_(const string &error_msg) noexcept {
            /**
             * 不接收可变参数，接受已经处理好的报错信息
             */

            cerr << "\n" << language::error::error_from << run_env::run_module << "\n" \
         << language::error::error_in_line << LINE_NOW + 1 << ":\n"\
         << error_msg << "\n";
        }
    }
}
