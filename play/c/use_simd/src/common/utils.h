#ifndef COMMON_UTILS_H_
#define COMMON_UTILS_H_

#include "muggle/c/base/macro.h"
#include <malloc.h>
#include <stddef.h>
#include <stdlib.h>

EXTERN_C_BEGIN

void *common_aligned_alloc(size_t alignment, size_t size);

void common_aligned_free(void *ptr);

EXTERN_C_END

#endif // !COMMON_UTILS_H_
