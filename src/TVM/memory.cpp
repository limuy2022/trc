﻿#include <TVM/TRE.hpp>
#include <TVM/TVM.hpp>
#include <TVM/memory.hpp>
#include <map>

namespace trc::TVM_space {
void free_TVM(TVM* vm) {
    vm->static_data.byte_codes.clear();
    vm->static_data.const_i.clear();
    vm->static_data.const_f.clear();

    while (vm->dyna_data.frames_top_str != vm->dyna_data.frames - 1) {
        vm->dyna_data.frames_top_str->free_func();
        vm->dyna_data.frames_top_str--;
    }
    vm->static_data.global_symbol_table_size = 1;
    vm->dyna_data.reset_global_symbol_table(1);
}

void TVM_quit_mem() {
    delete TVM_share::true_;
    delete TVM_share::false_;
    delete global_objs_pool;
}

objs_pool_TVM* global_objs_pool;

void init_mem() {
    global_objs_pool = new objs_pool_TVM;
    // 布尔值初始化
    TVM_share::true_ = new types::trc_int(1),
    TVM_share::false_ = new types::trc_int(0);
    // 整型缓存初始化，-5~256
    for (int i = INT_CACHE_BEGIN; i <= INT_CACHE_END; ++i)
        TVM_share::int_cache[i - INT_CACHE_BEGIN].value = i;
}
}
