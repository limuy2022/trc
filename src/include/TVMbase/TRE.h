#pragma once

#include <string>
#include "types/base.h"
#include "TVM/TVMdef.h"
#include "types/int.h"
#include "dll.h"

// int型缓存起始值
#define INT_CACHE_BEGIN -5
// int型缓存结束值
#define INT_CACHE_END 256
// int型缓存长度
#define INT_CACHE_LEN 262

using namespace std;

namespace trc
{
    namespace TVM_space
    {
        // 中间变量，便于使用
        TRC_TVMbase_api extern def::OBJ firstv, secondv;

        TRC_TVMbase_api extern def::INTOBJ firsti, secondi;

        TRC_TVMbase_api extern def::FLOATOBJ firstf, secondf;

        TRC_TVMbase_api extern def::STRINGOBJ firsts, seconds;

        TRC_TVMbase_api extern def::LONGOBJ firstl, secondl;

        namespace TRE
        {
            /**
             * Trc运行时环境
             */
            TRC_TVMbase_api extern NOARGV_TVM_METHOD TVM_RUN_CODE_NOARG_FUNC[];
            TRC_TVMbase_api extern ARGV_TVM_METHOD TVM_RUN_CODE_ARG_FUNC[];
        }

        namespace TVM_share
        {
            /**
             * TVM类型数据缓存
             */

            // 布尔值
            TRC_TVMbase_api extern def::INTOBJ true_, false_;
            // 整型缓存
            TRC_TVMbase_api extern types::trc_int int_cache[INT_CACHE_LEN];
        }

        namespace type_int
        {
            TRC_TVMbase_api extern map<string, int> name_int_s;
            TRC_TVMbase_api extern string int_name_s[];
        }
    }
}
