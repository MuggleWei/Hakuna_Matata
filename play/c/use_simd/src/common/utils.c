#include "utils.h"

void *common_aligned_alloc(size_t alignment, size_t size)
{
#if defined(_MSC_VER)
	retrn _aligned_malloc(size, alignment);
#else
	return aligned_alloc(alignment, size);
#endif
}

void common_aligned_free(void *ptr)
{
#if defined(_MSC_VER)
	_aligned_free(ptr);
#else
	free(ptr);
#endif
}
