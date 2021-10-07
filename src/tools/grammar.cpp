/**
 * 递归输出语法树，为一个调试功能
 */
#include <string>
#include <iostream>
#include <vector>
#include "../include/node.h"
#include "../include/read.h"
#include "../include/Compiler/Compiler.h"
#include "../include/memory/mem.h"
#include "../include/Compiler/compile_share.h"

using namespace std; 

static void out(treenode *data) {
    /*
    * 格式化输出语法树
    */
    cout << "[";
    switch(data->type) {
        case TREE:
            cout << "tree";
            break;
        case BUILTIN_FUNC:
            cout << "builtin_func";
            break;
        default:
            cout << data->data;
    }
    cout << ",";

    int n = (data->son).size();
    if (n)
        for (int i = 0; i < n; ++i)
            out((data->son)[i]);
    cout << "] ";
}

void __out_grammar(const string &path) {
    string file_data;
    readcode(file_data, path);
    // 解析
    vecs start_code_in;
    pre(start_code_in, file_data);
    auto token_code = final_token(start_code_in);

    start_code_in.clear();

    vector<treenode *> tree_node;
    grammar(tree_node, token_code);

    token_code.clear();

    cout << "From file " << path << ":" << "\n";

    size_t n = tree_node.size();
    for (int i = 0; i < n; ++i) {
        cout << i << ":";
        out(tree_node[i]);
        cout << "\n";
        free_tree(tree_node[i]);
    }
}

int out_grammar(int argc, char *argv[]) {
    /**
     * 输出语法树
     */ 
    for (int i = 2; i < argc; ++i)
        __out_grammar(argv[i]);
    return 0;
}
