#ifndef __MEMORY_POOL_H__
#define __MEMORY_POOL_H__

#include <stdint.h>

typedef struct MemoryPool_tag
{
    uintptr_t*      memory_pool_data_bufs;  // data buffer array
    uintptr_t*      memory_pool_ptr_buf;    // pointer buffer

    unsigned int    alloc_index;            // next time, alloc pointer index in pointer buffer
    unsigned int    free_index;             // next time, free pointer index in pointer buffer

    unsigned int    capacity;               // current memory pool capacity
    unsigned int    used;                   // how many block in use

    unsigned int    block_size;             // size of single block
    unsigned int    num_buf;                // the number of data buffer

#ifndef NDEBUG
    unsigned int    peak;                   // record max number of block in use
#endif
}MemoryPool;

void MemoryPoolInit(MemoryPool* pool, unsigned int init_capacity, unsigned int block_size);
void MemoryPoolDestroy(MemoryPool* pool);
uintptr_t MemoryPoolAlloc(MemoryPool* pool);
void MemoryPoolFree(MemoryPool* pool, void* p_data);

void MemoryPoolEnsureSpace(MemoryPool* pool, unsigned int capacity);

#endif