#pragma once

// 申请对象池
#define MALLOCINT trc::TVM_space::global_objs_pool->int_pool.trcmalloc
#define MALLOCFLOAT trc::TVM_space::global_objs_pool->float_pool.trcmalloc
#define MALLOCSTRING trc::TVM_space::global_objs_pool->str_pool.trcmalloc
#define MALLOCLONG trc::TVM_space::global_objs_pool->long_pool.trcmalloc
#define MALLOCFLONG trc::TVM_space::global_objs_pool->flong_pool.trcmalloc
