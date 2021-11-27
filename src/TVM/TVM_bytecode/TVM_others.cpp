#include "base/utils/filesys.h"
#include "Compiler/Compiler.h"
#include "base/Error.h"
#include "base/trcdef.h"
#include "TVMbase/TRE.h"
#include "TVM/TVM.h"
#include "base/utils/filesys.h"
#include "base/trcdef.h"
#include "base/memory/objs_pool.hpp"
#include "TVM/lib.h"
#include "base/utils/data.hpp"
#include "TVMbase/memory.h"

using namespace std;

namespace trc
{
    namespace TVM_space
    {
        void TVM::LOAD_INT(const short &index)
        {
            /**
             * 加载整型变量到栈
             */
            int value = static_data.const_i[index];
            if (INT_CACHE_BEGIN <= value && value <= INT_CACHE_END)
            {
                // 处在缓存范围中

                // 减去负数偏移量
                push(&TVM_share::int_cache[value - INT_CACHE_BEGIN]);
                return;
            }
            push(MALLOCINT(value));
        }

        void TVM::LOAD_MAP(const short &index)
        {
            /**
             * 加载map型变量到栈，参数含义是有几个参数
             */
        }

        void TVM::LOAD_FLOAT(const short &index)
        {
            /**
             * 加载浮点型变量到栈
             */
            push(MALLOCFLOAT(static_data.const_f[index]));
        }

        void TVM::LOAD_STRING(const short &index)
        {
            /**
             * 加载字符串变量到栈
             */
            push(MALLOCSTRING(static_data.const_s[index]));
        }

        void TVM::LOAD_LONG(const short &index)
        {
            /**
             * 加载大整数变量到栈
             */
            push(MALLOCLONG(static_data.const_long[index]));
        }

        void TVM::LOAD_ARRAY(const short &index)
        {
        }

        /* 什么都不做 */
        void TVM::NOP() {}

        void TVM::GOTO(const short &index)
        {
            /**
             * 跳转到行
             */
            run_env::lines[name] = static_data.const_i[index] - 1;
        }

        static void fix_path(const string &path)
        {
            /**
             * 检查路径是否正确
             * 如果路径不正确将路径转换成正确的，否则报错
             */

            if (utils::check_file_is(path))
            {
                return;
            }
            if (utils::check_in(path, cpp_libs::names))
            {
                return;
            }
            if (utils::check_file_is(utils::path_join(2, "TVM/packages/self_support", path.c_str())))
            {
                return;
            }
            if (utils::check_file_is(utils::path_join(2, "TVM/packages/other_support", path.c_str())))
            {
                return;
            }

            error::send_error(error::ModuleNotFoundError, path.c_str());
        }

        void TVM::IMPORT()
        {
            /**
             * 模块导入
             * 模块导入规则：
             * 首先检查是否为自定义模块
             * 其次检查是否为由系统原生支持的模块(由c++语言支持，定义在importlib动态链接库中)
             * 然后检查TVM/packages/self_support(由语言本身支持的模块)
             * 最后检查TVM/packages/other_support(由第三方支持的模块)
             */

            TVM *module;
            string codes, file_path, name_s;
            firsti = (def::INTOBJ)pop();
            int n = firsti->value;
            for (int i = 0; i < n; ++i)
            {
                firsts = (def::STRINGOBJ)pop();
                name_s = firsts->c_str();
                module = create_TVM(name_s);
                file_path = utils::import_to_path(name_s) + ".tree";
                fix_path(file_path);

                utils::readcode(codes, file_path);

                compiler::Compiler(module, codes);
                modules[name_s] = module;
                module->run();
            }
        }
    }
}
