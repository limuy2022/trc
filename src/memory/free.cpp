#include <map>
#include "TVM/TRE.h"
#include "TVM/TVM.h"
#include "memory/mem.hpp"
#include "utils/node.h"

using namespace std;

void free_module(map<string, TVM *> &modules) {
    /**
     * 递归删除TVM模块内存
     */
    for (const auto &i: modules)
        if (i.second->modules.empty())
            delete i.second;
        else
            free_module(i.second->modules);
}

void free_var_vm(TVM *vm) {
    /**
     * 删除TVM变量信息
     */

    const auto &end = vm->var_names.end();
    for (auto i = vm->var_names.begin(); i != end; ++i)
        delete i->second;
    vm->var_names.clear();
}

void quit_mem() {
    /**
     * 卸载内存，程序结束时使用
     */
    // 删除布尔缓存
    delete TVM_share::true_, TVM_share::false_;
    // 整形缓存
    for (auto & i : TVM_share::int_cache)
        delete i;
    delete memory::global_objs_pool;
}

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
