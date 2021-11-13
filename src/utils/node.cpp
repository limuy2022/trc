#include "node.h"

using namespace std;

void treenode::connect(treenode *son_connect) {
    /**
     * 本函数负责将父节点和子节点联系在一起
     */
    son_connect->father = this;
    son.push_back(son_connect);
}

treenode::treenode(int type_argv) :
        type(type_argv) {}

treenode::treenode(int type_argv, const string &data) :
        data(data),
        type(type_argv) {}
