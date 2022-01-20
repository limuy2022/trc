﻿#include "base/memory/memory.hpp"
#include "TVM/TVM.h"
#include "TVMbase/TRE.h"
#include "TVMbase/TVM_data.h"
#include "TVMbase/memory.h"
#include <map>

namespace trc::TVM_space {
void free_var_vm(TVM_dyna_data* data) {
    /**
     * 删除TVM变量信息
     */

    data->var_names.clear();
}

void free_TVM(TVM_space::TVM* vm) {
    /**
     * 删除TVM中的栈信息，常量池，字节码，但会保留变量信息（tshell）
     */

    memory::free_array_array(vm->static_data.byte_codes);
    vm->static_data.byte_codes.clear();
    vm->static_data.const_i.clear();
    vm->static_data.const_f.clear();
    vm->static_data.ReleaseStringData();
    memory::free_stl(vm->dyna_data.frames);
}

void quit_mem() {
    delete TVM_space::TVM_share::true_;
    delete TVM_space::TVM_share::false_;
    delete global_objs_pool;
}

objs_pool_TVM* global_objs_pool;

void init_mem() {
    global_objs_pool = new objs_pool_TVM;
    // 布尔值初始化
    TVM_space::TVM_share::true_
        = new TVM_space::types::trc_int(1),
        TVM_space::TVM_share::false_
        = new TVM_space::types::trc_int(0);
    // 整型缓存初始化，-5~256
    int index = 0;
    for (int i = INT_CACHE_BEGIN; i <= INT_CACHE_END; ++i)
        TVM_space::TVM_share::int_cache[index++].value = i;
    TVM_space::firsti = MALLOCINT(),
    TVM_space::secondi = MALLOCINT();
    TVM_space::firstf = MALLOCFLOAT(),
    TVM_space::secondf = MALLOCFLOAT();
    TVM_space::firsts = MALLOCSTRING(),
    TVM_space::seconds = MALLOCSTRING();
    TVM_space::firstl = MALLOCLONG(),
    TVM_space::secondl = MALLOCLONG();
}
}