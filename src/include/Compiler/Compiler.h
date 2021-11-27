/**
 * 编译器所有供外调的函数
 */

#pragma once

#include "base/trcdef.h"
#include "compile_def.hpp"
#include "TVM/TVM.h"
#include "dll.h"

using namespace std;

namespace trc
{
    namespace compiler
    {
        class grammar_data_control;
        class detail_grammar;

        class TRC_Compiler_api token_lex
        {
        public:
            token_lex(const string &code);

            ~token_lex();

            vecs get_line();

        private:
            const string code;

            // 当前正在解析的行
            int *line;
            // 上一行的换行符的索引
            char *char_ptr;
        };

        class TRC_Compiler_api grammar_lex
        {
        public:
            grammar_lex(const string &codes_str);

            ~grammar_lex();

            vector<treenode *> *compile_nodes();

        private:
            token_lex token_;

            grammar_data_control *env;

            // 这是解析的实现细节
            detail_grammar *lex;

            vector<treenode *> codes;
        };

        class TRC_Compiler_api compiler_t
        {
        public:
            void operator()(TVM_space::TVM *vm, const string &codes);

        private:
            // 语法解析器
            grammar_lex *grammar_;
            vector<treenode *> *gline;
            int *lex_num;
        };

        TRC_Compiler_api extern compiler_t Compiler;

        TRC_Compiler_api void free_tree(const compiler::treenode *head);
    }
}
