﻿#pragma once

#include <TVMbase/types/base.h>
#include <TVMbase/types/float.h>
#include <TVMbase/types/flong.h>
#include <TVMbase/types/int.h>
#include <TVMbase/types/long.h>
#include <TVMbase/types/string.h>
#include <base/library.h>
#include <base/memory/objs_pool.hpp>
#include <platform.h>

// 申请对象池
#define MALLOCINT trc::TVM_space::global_objs_pool->int_pool.trcmalloc
#define MALLOCFLOAT trc::TVM_space::global_objs_pool->float_pool.trcmalloc
#define MALLOCSTRING trc::TVM_space::global_objs_pool->str_pool.trcmalloc
#define MALLOCLONG trc::TVM_space::global_objs_pool->long_pool.trcmalloc
#define MALLOCFLONG trc::TVM_space::global_objs_pool->flong_pool.trcmalloc

namespace trc::TVM_space {
struct TVM_dyna_data;
struct TRC_TVMbase_api objs_pool_TVM {
    /**
     * 对对象池的基本封装，对象池面向全体对象
     * 而这个类面向TVM用于保存基础类型
     */
    memory::objs_pool<types::trc_int> int_pool;
    memory::objs_pool<types::trc_float> float_pool;
    memory::objs_pool<types::trc_string> str_pool;
    memory::objs_pool<types::trc_long> long_pool;
    memory::objs_pool<types::trc_flong> flong_pool;
};

/**
 * @brief 删除TVM变量信息
 */
TRC_TVMbase_func_api void free_var_vm(TVM_dyna_data* vm);

/**
 * @brief 删除TVM中的栈信息，常量池，字节码
 * @warning 会保留变量信息（tshell）
 */
TRC_TVMbase_func_api void free_TVM(TVM* vm);

TRC_TVMbase_func_api void quit_mem();

TRC_TVMbase_func_api void quit_mem();

TRC_TVMbase_func_api void init_mem();

TRC_TVMbase_api extern objs_pool_TVM* global_objs_pool;
}
