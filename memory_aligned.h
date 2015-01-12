#ifndef MEMORY_ALIGNED_H_
#define MEMORY_ALIGNED_H_

#define POWER_OF_TWO_SIZEOF(size, n)	( (size + n - 1) & ~(n - 1) )
#define MEMORY_ALIGNED(ptr, n)			( ((uintptr_t)ptr + n - 1) & ~(uintptr_t)(n-1) )

#endif
