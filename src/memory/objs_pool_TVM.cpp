/**
 * TVM所需类型的对象池的集合
 */ 
#include "memory/objs_pool.hpp"

using namespace std;

objs_pool_TVM::objs_pool_TVM(size_t init_size):
    int_pool(new objs_pool<trc_int>(init_size)),
    float_pool(new objs_pool<trc_float>(init_size)),
    str_pool(new objs_pool<trc_string>(init_size)),
    long_pool(new objs_pool<trc_long>(init_size))
{}

objs_pool_TVM::~objs_pool_TVM() {
    delete int_pool, float_pool, str_pool, long_pool;
}
