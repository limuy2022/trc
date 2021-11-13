/**
 * 编译器需要使用的释放内存的模块
 * 但这些函数并不是只有编译器才能调用
 */
#include "node.h"
#include "memory/mem.hpp"
#include "TVM/TVM.h"

using namespace std;

void free_tree(const treenode *head) {
    /**
    * 释放整棵树的内存
    * 递归释放内存
    * delete
    */
    for (const auto &i: head->son)
        if (!i->son.empty())
            free_tree(i);
        else
            delete i;
    delete head;
    head = nullptr;
}


void free_TVM(TVM *vm) {
    /**
     * 删除TVM中的栈信息，常量池，字节码，但会保留变量信息（tshell）
     */

    for (auto &i: vm->static_data.byte_codes)
        free_vector(i);
    vm->static_data.byte_codes.clear();
    vm->static_data.const_i.clear();
    vm->static_data.const_f.clear();
    vm->static_data.const_s.clear();
    vm->static_data.const_name.clear();
    vm->static_data.const_long.clear();
    free_stl(vm->frames);
}
