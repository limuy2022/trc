/**
 * @file pri_compiler.h
 * @brief Compiler模块的私有成员
 * @date 2022-01-18
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include <string>
using namespace std;

namespace trc
{
    namespace compiler
    {
        enum token_ticks
        {
            VAR,
            STRING_VALUE,
            FLOAT_VALUE,
            INT_VALUE,
            RIGHT_IF_BRACE,
            RIGHT_WHILE_BRACE,
            FOR,
            WHILE,
        };

        struct token
        {
            /**
             * 一个完整的token包括标识和值两部分
             */
            // 标识
            token_ticks tick;
            // 值
            string data;
        };
    }
}
