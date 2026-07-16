#include "matrix_avx2.h"
#include "muggle/c/log/log.h"
#include <stdint.h>

#if SIMD_HAS_AVX2

	#include <immintrin.h>

	#define AVX_ALIGNMENT (32llu)

void simd_avx2_matrix4_add(const matrix4_t *mat1, const matrix4_t *mat2,
						   matrix4_t *mat)
{
	MUGGLE_ASSERT_MSG(((uintptr_t)mat1 & (AVX_ALIGNMENT - 1)) == 0,
					  "matrix1 not aligned 32 bytes");
	MUGGLE_ASSERT_MSG(((uintptr_t)mat2 & (AVX_ALIGNMENT - 1)) == 0,
					  "matrix2 not aligned 32 bytes");
	MUGGLE_ASSERT_MSG(((uintptr_t)mat & (AVX_ALIGNMENT - 1)) == 0,
					  "matrix not aligned 32 bytes");

	__m256 a0 = _mm256_load_ps((const float *)mat1 + 0);
	__m256 a1 = _mm256_load_ps((const float *)mat1 + 8);

	__m256 b0 = _mm256_load_ps((const float *)mat2 + 0);
	__m256 b1 = _mm256_load_ps((const float *)mat2 + 8);

	_mm256_store_ps((float *)mat + 0, _mm256_add_ps(a0, b0));
	_mm256_store_ps((float *)mat + 8, _mm256_add_ps(a1, b1));
}

void simd_avx2_matrix4_mul(const matrix4_t *mat1, const matrix4_t *mat2,
						   matrix4_t *mat)
{
	MUGGLE_ASSERT_MSG(((uintptr_t)mat1 & (AVX_ALIGNMENT - 1)) == 0,
					  "matrix1 not aligned 32 bytes");
	MUGGLE_ASSERT_MSG(((uintptr_t)mat2 & (AVX_ALIGNMENT - 1)) == 0,
					  "matrix2 not aligned 32 bytes");
	MUGGLE_ASSERT_MSG(((uintptr_t)mat & (AVX_ALIGNMENT - 1)) == 0,
					  "matrix not aligned 32 bytes");

	// b_00 = [b00, b01, b02, b03, b00, b01, b02, b03]
	__m256 b_00 =
		_mm256_broadcast_ps((const __m128 *)((const float *)mat2 + 0));
	// b_11 = [b10, b11, b12, b13, b10, b11, b12, b13]
	__m256 b_11 =
		_mm256_broadcast_ps((const __m128 *)((const float *)mat2 + 4));
	// b_22 = [b20, b21, b22, b23, b20, b21, b22, b23]
	__m256 b_22 =
		_mm256_broadcast_ps((const __m128 *)((const float *)mat2 + 8));
	// b_33 = [b30, b31, b32, b33, b30, b31, b32, b33]
	__m256 b_33 =
		_mm256_broadcast_ps((const __m128 *)((const float *)mat2 + 12));

#if !INCREASE_SIMD_ILP 

	// --------------------------------
	// calculate matrix row 0 and row 1
	// --------------------------------

	// a_01 = [a00, a01, a02, a03, a10, a11, a12, a12]
	__m256 a_01 = _mm256_load_ps((const float *)mat1 + 0);

	// a_01_0 = [a00, a00, a00, a00, a10, a10, a10, a10]
	__m256 a_01_0 = _mm256_shuffle_ps(a_01, a_01, _MM_SHUFFLE(0, 0, 0, 0));
	// a_01_1 = [a01, a01, a01, a01, a11, a11, a11, a11]
	__m256 a_01_1 = _mm256_shuffle_ps(a_01, a_01, _MM_SHUFFLE(1, 1, 1, 1));
	// a_01_2 = [a02, a02, a02, a02, a12, a12, a12, a12]
	__m256 a_01_2 = _mm256_shuffle_ps(a_01, a_01, _MM_SHUFFLE(2, 2, 2, 2));
	// a_01_3 = [a03, a03, a03, a03, a13, a13, a13, a13]
	__m256 a_01_3 = _mm256_shuffle_ps(a_01, a_01, _MM_SHUFFLE(3, 3, 3, 3));

	// C_row = a_01_0 * b_00 + a_01_1 * b_11 + a_01_2 * b_22 + a_01_3 * b_33
	// = [
	//   a0k * bk0, a0k * bk1, a0k * bk2, a0k * bk3,
	//   a1k * bk0, a1k * bk2, a1k * bk2, a1k * bk3
	// ]
	__m256 c_01 = _mm256_mul_ps(a_01_0, b_00);
	c_01 = _mm256_fmadd_ps(a_01_1, b_11, c_01);
	c_01 = _mm256_fmadd_ps(a_01_2, b_22, c_01);
	c_01 = _mm256_fmadd_ps(a_01_3, b_33, c_01);

	_mm256_store_ps((float *)mat + 0, c_01);

	// --------------------------------
	// calculate matrix row 2 and row 3
	// --------------------------------

	__m256 a_23 = _mm256_load_ps((const float *)mat1 + 8);

	__m256 a_23_0 = _mm256_shuffle_ps(a_23, a_23, _MM_SHUFFLE(0, 0, 0, 0));
	__m256 a_23_1 = _mm256_shuffle_ps(a_23, a_23, _MM_SHUFFLE(1, 1, 1, 1));
	__m256 a_23_2 = _mm256_shuffle_ps(a_23, a_23, _MM_SHUFFLE(2, 2, 2, 2));
	__m256 a_23_3 = _mm256_shuffle_ps(a_23, a_23, _MM_SHUFFLE(3, 3, 3, 3));

	__m256 c_23 = _mm256_mul_ps(a_23_0, b_00);
	c_23 = _mm256_fmadd_ps(a_23_1, b_11, c_23);
	c_23 = _mm256_fmadd_ps(a_23_2, b_22, c_23);
	c_23 = _mm256_fmadd_ps(a_23_3, b_33, c_23);

	_mm256_store_ps((float *)mat + 8, c_23);

#else

	__m256 a_01 = _mm256_load_ps((const float *)mat1 + 0);
	__m256 a_01_0 = _mm256_shuffle_ps(a_01, a_01, _MM_SHUFFLE(0, 0, 0, 0));
	__m256 a_01_1 = _mm256_shuffle_ps(a_01, a_01, _MM_SHUFFLE(1, 1, 1, 1));
	__m256 a_01_2 = _mm256_shuffle_ps(a_01, a_01, _MM_SHUFFLE(2, 2, 2, 2));
	__m256 a_01_3 = _mm256_shuffle_ps(a_01, a_01, _MM_SHUFFLE(3, 3, 3, 3));

	__m256 a_23 = _mm256_load_ps((const float *)mat1 + 8);
	__m256 a_23_0 = _mm256_shuffle_ps(a_23, a_23, _MM_SHUFFLE(0, 0, 0, 0));
	__m256 a_23_1 = _mm256_shuffle_ps(a_23, a_23, _MM_SHUFFLE(1, 1, 1, 1));
	__m256 a_23_2 = _mm256_shuffle_ps(a_23, a_23, _MM_SHUFFLE(2, 2, 2, 2));
	__m256 a_23_3 = _mm256_shuffle_ps(a_23, a_23, _MM_SHUFFLE(3, 3, 3, 3));

	__m256 c_01 = _mm256_mul_ps(a_01_0, b_00);
	__m256 c_23 = _mm256_mul_ps(a_23_0, b_00);

	c_01 = _mm256_fmadd_ps(a_01_1, b_11, c_01);
	c_23 = _mm256_fmadd_ps(a_23_1, b_11, c_23);

	c_01 = _mm256_fmadd_ps(a_01_2, b_22, c_01);
	c_23 = _mm256_fmadd_ps(a_23_2, b_22, c_23);

	c_01 = _mm256_fmadd_ps(a_01_3, b_33, c_01);
	c_23 = _mm256_fmadd_ps(a_23_3, b_33, c_23);

	_mm256_store_ps((float *)mat + 0, c_01);
	_mm256_store_ps((float *)mat + 8, c_23);

#endif
}

#endif
