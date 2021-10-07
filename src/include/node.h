#ifndef TRC_UTILS_NODE_H
#define TRC_UTILS_NODE_H

#include <string>
#include <vector>
#include "Compiler/compile_share.h"

using namespace std;

class treenode {
    /**
     * 树节点，生成语法分析树时用到
     */
public:
    treenode *father = nullptr;
    string data;
    int type;
    vector<treenode *> son;

    treenode(int type_argv = TREE);

    treenode(int type_argv, const string &data);

    void connect(treenode *son_connect);
};

#endif
