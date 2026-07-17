#include "matrix.h"

void matrix4_add(const matrix4_t *mat1, const matrix4_t *mat2, matrix4_t *mat)
{
#if defined(__GNUC__) || defined(__clang__)
	const float *a = (const float *)__builtin_assume_aligned(mat1, 64);
	const float *b = (const float *)__builtin_assume_aligned(mat2, 64);
	float *c = (float*)__builtin_assume_aligned(mat, 64);
#else
	const float *a = (const float *)mat1;
	const float *b = (const float *)mat2;
	float *c = (float *)mat;
#endif

	for (int i = 0; i < 16; ++i) {
		c[i] = a[i] + b[i];
	}
}

void matrix4_mul(const matrix4_t *mat1, const matrix4_t *mat2, matrix4_t *mat)
{
#if defined(__GNUC__) || defined(__clang__)
	const float *a = (const float *)__builtin_assume_aligned(mat1, 64);
	const float *b = (const float *)__builtin_assume_aligned(mat2, 64);
	float *c = (float*)__builtin_assume_aligned(mat, 64);
#else
	const float *a = (const float *)mat1;
	const float *b = (const float *)mat2;
	float *c = (float *)mat;
#endif

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			float sum = 0.0f;
			for (int k = 0; k < 4; ++k) {
				sum += a[i * 4 + k] * b[k * 4 + j];
			}
			c[i * 4 + j] = sum;
		}
	}
}
