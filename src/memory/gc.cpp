/**
 * 垃圾回收算法采用引用计数回收
 * 至于相互引用的漏洞，将会采用可达性回收算法进行
 *
 * 注意，在垃圾回收中，所有虚拟机中的对象并不存放，而是统一继承于trcobj，以便在TVM析构时进行垃圾回收
 *
 * 回收条件：
 * 1.当申请的内存达到gc_obj所允许的最大值时，进行垃圾回收
 * 2.当单个对象池处于满状态时
 * 此次进行的是较浅的垃圾回收
 * 注：在此种情况下如果仍然不能满足需求，则扩大gc_obj允许的字节数
 * 
 * 当垃圾回收运行时，字节码运行也暂停
 * 
 * 垃圾回收直接由gc_obj完成，由gc_obj链接objs_pool达到
 */

#include "../include/memory/mem.h"
#include "../include/memory/objs_pool.hpp"

using namespace std;

gc_obj::gc_obj():
    pool_(new objs_pool_TVM(this))
{}

#define gc_one_pool(now, _pool_)\
do{\
    while((now) != nullptr) {\
        if((now) ->data-> refs == 0) {\
            _pool_->used_head -> next = (now) -> next;\
            now -> next = _pool_->free_head -> next;\
            _pool_->free_head -> next = (now);\
            (now) -> data -> delete_();\
        }\
    }\
} while(0)

void gc_obj::gc() {
    /**
     * 垃圾回收
     */
    objs_pool<trcint>::node_ *now1 = pool_-> int_pool -> used_head;
    objs_pool<trcfloat>::node_ *now2 = pool_-> float_pool -> used_head;
    objs_pool<trc_string>::node_ *now3 = pool_-> str_pool -> used_head;
    objs_pool<BigNum>::node_ *now4 = pool_-> long_pool -> used_head;
    gc_one_pool(now1, pool_-> int_pool);
    gc_one_pool(now2, pool_-> float_pool);
    gc_one_pool(now3, pool_-> str_pool);
    gc_one_pool(now4, pool_-> long_pool);
}

gc_obj::~gc_obj() {
    /**
     * 清除被标记的所有对象，通常在析构时调用
     */
    delete pool_;
}

objs_pool_TVM::objs_pool_TVM(gc_obj* con):
    int_pool(new objs_pool<trcint>(con)),
    float_pool(new objs_pool<trcfloat>(con)),
    str_pool(new objs_pool<trc_string>(con)),
    long_pool(new objs_pool<BigNum>(con))
{}

objs_pool_TVM::~objs_pool_TVM() {
    delete int_pool, float_pool, str_pool, long_pool;
}
