#include <cstdlib>
#include "../include/memory/memory_pool.h"

using namespace std;

namespace memory_pool_class{
    class node {

    };
    
    class freespace {

    };
    
}

void *memory_pool::mem_realoc(void *p, size_t size_) {

}

void *memory_pool::mem_malloc(size_t size_) {

}

void memory_pool::mem_free(void *p) {
	free(mem_base);
}

memory_pool::memory_pool(size_t size) {
	// 需要realloc
	mem_base = (char*)(malloc(sizeof(char) * MEMORY_INIT_SIZE));
}
