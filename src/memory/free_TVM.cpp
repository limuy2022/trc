#include <map>
#include "../include/TVM/TRE.h"
#include "../include/TVM/bignum.h"
#include "../include/TVM/int.h"
#include "../include/TVM/TVM.h"
#include "../include/memory/mem.hpp"

using namespace std;

void free_module(map<string, TVM *> &modules) {
    /**
     * 递归删除TVM模块内存
     */
    for (const auto& i : modules)
        if (i.second->modules.empty())
            delete i.second;
        else
            free_module(i.second->modules);
}

void free_var_vm(TVM *vm) {
    /**
     * 删除TVM变量信息
     */ 

    auto end = vm->var_names.end();
    for(auto i = vm->var_names.begin(); i != end; ++i)
        delete i->second;
    vm->var_names.clear();
}

void quit_mem() {
    delete TVM_share::true_, TVM_share::false_;
    for(int i = 0; i < INT_CACHE_LEN; ++i)
        delete TVM_share::int_cache[i];
    delete memory::global_gc_obj, \
    memory::global_objs_pool, \
    memory::global_memory_pool;
}
