#pragma once

#include <TVM/types/base.hpp>
#include <TVM/types/trc_float.hpp>
#include <TVM/types/trc_flong.hpp>
#include <TVM/types/trc_int.hpp>
#include <TVM/types/trc_long.hpp>
#include <TVM/types/trc_string.hpp>
#include <base/memory/objs_pool.hpp>

// 申请对象池
#define MALLOCINT trc::TVM_space::global_objs_pool->int_pool.trcmalloc
#define MALLOCFLOAT trc::TVM_space::global_objs_pool->float_pool.trcmalloc
#define MALLOCSTRING trc::TVM_space::global_objs_pool->str_pool.trcmalloc
#define MALLOCLONG trc::TVM_space::global_objs_pool->long_pool.trcmalloc
#define MALLOCFLONG trc::TVM_space::global_objs_pool->flong_pool.trcmalloc

namespace trc::TVM_space {
class TVM_dyna_data;
/**
 * 对对象池的基本封装，对象池面向全体对象
 * 而这个类面向TVM用于保存基础类型
 */
struct objs_pool_TVM {
    memory::objs_pool<types::trc_int> int_pool;
    memory::objs_pool<types::trc_float> float_pool;
    memory::objs_pool<types::trc_string> str_pool;
    memory::objs_pool<types::trc_long> long_pool;
    memory::objs_pool<types::trc_flong> flong_pool;
};

/**
 * @brief 删除TVM中的栈信息，常量池，字节码
 * @warning 会保留变量信息（tshell）
 */
void free_TVM(TVM* vm);

void TVM_quit_mem();

void init_mem();

extern objs_pool_TVM* global_objs_pool;
}
