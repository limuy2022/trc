/**
 * 
 */ 

#include <cstdlib>
#include "../include/memory/memory_pool.h"

using namespace std;

namespace memory_pool_class{
    class node {
        void *address;
    };
    
    class freespace {

    };
    
}

void *memory_pool::mem_realoc(void *p, size_t size_) {
    /**
     * 重新设置大小
     */ 
    return realloc(p, size_);
}

void *memory_pool::mem_malloc(size_t size_) {
    /**
     * 申请内存
     */ 
    return malloc(size_);
}

void memory_pool::mem_free(void *p) {
    /**
     * 释放内存
     */ 
	free(p);
}

memory_pool::memory_pool(size_t size) {
	// 需要realloc
	mem_base = (char*)(malloc(sizeof(char) * MEMORY_INIT_SIZE));
}

memory_pool::~memory_pool() {
    free(mem_base);
}
