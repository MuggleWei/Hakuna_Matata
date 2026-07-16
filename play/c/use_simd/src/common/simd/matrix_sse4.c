#include "matrix_sse4.h"
#include "muggle/c/log/log.h"
#include <stdint.h>

#if SIMD_HAS_SSE4

	#include <immintrin.h>

	#define SSE_ALIGNMENT (16llu)
	#define M128_BROADCAST(v, i) _mm_shuffle_ps(v, v, _MM_SHUFFLE(i, i, i, i))

void simd_sse4_matrix4_add(const matrix4_t *mat1, const matrix4_t *mat2,
						   matrix4_t *mat)
{
	MUGGLE_ASSERT_MSG(((uintptr_t)mat1 & (SSE_ALIGNMENT - 1)) == 0,
					  "matrix1 not aligned 16 bytes");
	MUGGLE_ASSERT_MSG(((uintptr_t)mat2 & (SSE_ALIGNMENT - 1)) == 0,
					  "matrix2 not aligned 16 bytes");
	MUGGLE_ASSERT_MSG(((uintptr_t)mat & (SSE_ALIGNMENT - 1)) == 0,
					  "matrix not aligned 16 bytes");

	__m128 a0 = _mm_load_ps((const float *)mat1 + 0);
	__m128 a1 = _mm_load_ps((const float *)mat1 + 4);
	__m128 a2 = _mm_load_ps((const float *)mat1 + 8);
	__m128 a3 = _mm_load_ps((const float *)mat1 + 12);

	__m128 b0 = _mm_load_ps((const float *)mat2 + 0);
	__m128 b1 = _mm_load_ps((const float *)mat2 + 4);
	__m128 b2 = _mm_load_ps((const float *)mat2 + 8);
	__m128 b3 = _mm_load_ps((const float *)mat2 + 12);

	_mm_store_ps((float *)mat + 0, _mm_add_ps(a0, b0));
	_mm_store_ps((float *)mat + 4, _mm_add_ps(a1, b1));
	_mm_store_ps((float *)mat + 8, _mm_add_ps(a2, b2));
	_mm_store_ps((float *)mat + 12, _mm_add_ps(a3, b3));
}

void simd_sse4_matrix4_mul(const matrix4_t *mat1, const matrix4_t *mat2,
						   matrix4_t *mat)
{
	MUGGLE_ASSERT_MSG(((uintptr_t)mat1 & (SSE_ALIGNMENT - 1)) == 0,
					  "matrix1 not aligned 16 bytes");
	MUGGLE_ASSERT_MSG(((uintptr_t)mat2 & (SSE_ALIGNMENT - 1)) == 0,
					  "matrix2 not aligned 16 bytes");
	MUGGLE_ASSERT_MSG(((uintptr_t)mat & (SSE_ALIGNMENT - 1)) == 0,
					  "matrix not aligned 16 bytes");

	__m128 a0 = _mm_load_ps((const float *)mat1 + 0);
	__m128 a1 = _mm_load_ps((const float *)mat1 + 4);
	__m128 a2 = _mm_load_ps((const float *)mat1 + 8);
	__m128 a3 = _mm_load_ps((const float *)mat1 + 12);

	__m128 b0 = _mm_load_ps((const float *)mat2 + 0);
	__m128 b1 = _mm_load_ps((const float *)mat2 + 4);
	__m128 b2 = _mm_load_ps((const float *)mat2 + 8);
	__m128 b3 = _mm_load_ps((const float *)mat2 + 12);

	__m128 c0 = _mm_setzero_ps();
	__m128 c1 = _mm_setzero_ps();
	__m128 c2 = _mm_setzero_ps();
	__m128 c3 = _mm_setzero_ps();

#if !INCREASE_SIMD_ILP 

	c0 = _mm_mul_ps(M128_BROADCAST(a0, 0), b0);
	c0 = _mm_add_ps(c0, _mm_mul_ps(M128_BROADCAST(a0, 1), b1));
	c0 = _mm_add_ps(c0, _mm_mul_ps(M128_BROADCAST(a0, 2), b2));
	c0 = _mm_add_ps(c0, _mm_mul_ps(M128_BROADCAST(a0, 3), b3));

	c1 = _mm_mul_ps(M128_BROADCAST(a1, 0), b0);
	c1 = _mm_add_ps(c1, _mm_mul_ps(M128_BROADCAST(a1, 1), b1));
	c1 = _mm_add_ps(c1, _mm_mul_ps(M128_BROADCAST(a1, 2), b2));
	c1 = _mm_add_ps(c1, _mm_mul_ps(M128_BROADCAST(a1, 3), b3));

	c2 = _mm_mul_ps(M128_BROADCAST(a2, 0), b0);
	c2 = _mm_add_ps(c2, _mm_mul_ps(M128_BROADCAST(a2, 1), b1));
	c2 = _mm_add_ps(c2, _mm_mul_ps(M128_BROADCAST(a2, 2), b2));
	c2 = _mm_add_ps(c2, _mm_mul_ps(M128_BROADCAST(a2, 3), b3));

	c3 = _mm_mul_ps(M128_BROADCAST(a3, 0), b0);
	c3 = _mm_add_ps(c3, _mm_mul_ps(M128_BROADCAST(a3, 1), b1));
	c3 = _mm_add_ps(c3, _mm_mul_ps(M128_BROADCAST(a3, 2), b2));
	c3 = _mm_add_ps(c3, _mm_mul_ps(M128_BROADCAST(a3, 3), b3));

#else

	c0 = _mm_mul_ps(M128_BROADCAST(a0, 0), b0);
	c1 = _mm_mul_ps(M128_BROADCAST(a1, 0), b0);
	c2 = _mm_mul_ps(M128_BROADCAST(a2, 0), b0);
	c3 = _mm_mul_ps(M128_BROADCAST(a3, 0), b0);

	c0 = _mm_add_ps(c0, _mm_mul_ps(M128_BROADCAST(a0, 1), b1));
	c1 = _mm_add_ps(c1, _mm_mul_ps(M128_BROADCAST(a1, 1), b1));
	c2 = _mm_add_ps(c2, _mm_mul_ps(M128_BROADCAST(a2, 1), b1));
	c3 = _mm_add_ps(c3, _mm_mul_ps(M128_BROADCAST(a3, 1), b1));

	c0 = _mm_add_ps(c0, _mm_mul_ps(M128_BROADCAST(a0, 2), b2));
	c1 = _mm_add_ps(c1, _mm_mul_ps(M128_BROADCAST(a1, 2), b2));
	c2 = _mm_add_ps(c2, _mm_mul_ps(M128_BROADCAST(a2, 2), b2));
	c3 = _mm_add_ps(c3, _mm_mul_ps(M128_BROADCAST(a3, 2), b2));

	c0 = _mm_add_ps(c0, _mm_mul_ps(M128_BROADCAST(a0, 3), b3));
	c1 = _mm_add_ps(c1, _mm_mul_ps(M128_BROADCAST(a1, 3), b3));
	c2 = _mm_add_ps(c2, _mm_mul_ps(M128_BROADCAST(a2, 3), b3));
	c3 = _mm_add_ps(c3, _mm_mul_ps(M128_BROADCAST(a3, 3), b3));

#endif

	// store result
	_mm_store_ps((float *)mat + 0, c0);
	_mm_store_ps((float *)mat + 4, c1);
	_mm_store_ps((float *)mat + 8, c2);
	_mm_store_ps((float *)mat + 12, c3);
}

#endif
