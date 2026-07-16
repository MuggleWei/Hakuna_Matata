#ifndef COMMON_MATRIX_NEON_H_
#define COMMON_MATRIX_NEON_H_

#include "muggle/c/base/macro.h"
#include "common/config.h"
#include "common/matrix.h"

EXTERN_C_BEGIN

#if SIMD_HAS_NEON

void simd_neon_matrix4_add(const matrix4_t *mat1, const matrix4_t *mat2,
						   matrix4_t *mat);

void simd_neon_matrix4_mul(const matrix4_t *mat1, const matrix4_t *mat2,
						   matrix4_t *mat);

#endif

EXTERN_C_END

#endif // !COMMON_MATRIX_NEON_H_
