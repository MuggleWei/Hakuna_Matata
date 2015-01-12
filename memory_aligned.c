#include "memory_aligned.h"
#include <stdlib.h>
#include <stdint.h>

uintptr_t AlignedAlloc(size_t size, size_t alignment, void** alloc_ptr)
{
	uintptr_t aligned_ptr;

	*alloc_ptr = malloc(size + alignment - 1);
	aligned_ptr = MEMORY_ALIGNED(alloc_ptr, alignment);
	return aligned_ptr;
}