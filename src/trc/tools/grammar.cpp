/**
 * 递归输出语法树，为一个调试功能
 */
#include <string>
#include <iostream>
#include <vector>
#include "base/utils/filesys.h"
#include "Compiler/Compiler.h"
#include "base/memory/memory.h"

using namespace std;

namespace trc
{
    namespace tools_in
    {
        void out(compiler::treenode *data)
        {
            /*
             * 格式化输出语法树
             */
            cout << "[";
            switch (data->type)
            {
            case compiler::TREE:
                cout << "tree";
                break;
            case compiler::BUILTIN_FUNC:
                cout << "builtin_func";
                break;
            default:
                cout << data->data;
            }
            cout << ",";
            if (int n = (data->son).size())
                for (int i = 0; i < n; ++i)
                    out((data->son)[i]);
            cout << "] ";
        }

        void __out_grammar(const string &path)
        {
            string file_data;
            utils::readcode(file_data, path);
            compiler::grammar_lex grammar_t(file_data);

            vector<compiler::treenode *> *tree_node = grammar_t.compile_nodes();

            cout << "From file " << path << ":"
                 << "\n";

            for (size_t i = 0, n = tree_node->size(); i < n; ++i)
            {
                cout << i << ":";
                out((*tree_node)[i]);
                cout << "\n";
                compiler::free_tree((*tree_node)[i]);
            }
        }
    }

    namespace tools_out
    {
        void out_grammar(int argc, char *argv[])
        {
            /**
             * 输出语法树
             */
            for (int i = 2; i < argc; ++i)
                tools_in::__out_grammar(argv[i]);
        }
    }
}
