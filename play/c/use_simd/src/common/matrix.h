#ifndef COMMON_MATRIX_H_
#define COMMON_MATRIX_H_

#include "muggle/c/base/macro.h"

EXTERN_C_BEGIN

typedef struct {
	float vals[16];
} matrix4_t;

void matrix4_add(const matrix4_t *mat1, const matrix4_t *mat2, matrix4_t *mat);

void matrix4_mul(const matrix4_t *mat1, const matrix4_t *mat2, matrix4_t *mat);

EXTERN_C_END

#endif // !COMMON_MATRIX_H_
