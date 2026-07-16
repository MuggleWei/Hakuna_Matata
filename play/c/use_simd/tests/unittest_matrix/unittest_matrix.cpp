#include "common/utils.h"
#include "unity.h"
#include "common/config.h"
#include "common/matrix.h"
#include "common/simd/matrix_sse4.h"
#include "common/simd/matrix_avx2.h"
#include "common/simd/matrix_neon.h"
#include <time.h>
#include <string.h>

matrix4_t *s_mat_arr = NULL;

void setUp()
{
	srand(time(NULL));
	s_mat_arr = (matrix4_t *)common_aligned_alloc(64, sizeof(matrix4_t) * 4);
}

void tearDown()
{
	common_aligned_free(s_mat_arr);
}

static void check_matrix_equal(const float *actual, const float *expect)
{
	for (int i = 0; i < 16; ++i) {
		float epsilon = 0.01f;
		TEST_ASSERT_TRUE(fabsf(expect[i] - actual[i]) < epsilon);
	}
}

static void test_add(float *mat1, float *mat2, float *mat_result)
{
	float *a = (float *)(s_mat_arr + 0);
	float *b = (float *)(s_mat_arr + 1);
	float *c = (float *)(s_mat_arr + 2);
	float *expect_c = (float *)(s_mat_arr + 3);
	memcpy(a, mat1, sizeof(matrix4_t));
	memcpy(b, mat2, sizeof(matrix4_t));
	memcpy(expect_c, mat_result, sizeof(matrix4_t));

	memset(c, 0, sizeof(matrix4_t));
	matrix4_add((const matrix4_t *)a, (const matrix4_t *)b, (matrix4_t *)c);
	check_matrix_equal(c, expect_c);

#if SIMD_HAS_SSE4
	memset(c, 0, sizeof(matrix4_t));
	simd_sse4_matrix4_add((const matrix4_t *)a, (const matrix4_t *)b,
						  (matrix4_t *)c);
	check_matrix_equal(c, expect_c);
#endif

#if SIMD_HAS_AVX2
	memset(c, 0, sizeof(matrix4_t));
	simd_avx2_matrix4_add((const matrix4_t *)a, (const matrix4_t *)b,
						  (matrix4_t *)c);
	check_matrix_equal(c, expect_c);
#endif

#if SIMD_HAS_NEON
	memset(c, 0, sizeof(matrix4_t));
	simd_neon_matrix4_add((const matrix4_t *)a, (const matrix4_t *)b,
						  (matrix4_t *)c);
	check_matrix_equal(c, expect_c);
#endif
}

void matrix_add_case01()
{
	float mat1[16] = {
		1, 1, 1, 1, //
		0, 0, 0, 0, //
		1, 1, 1, 1, //
		0, 0, 0, 0 //
	};
	float mat2[16] = {
		0, 0, 0, 0, //
		1, 1, 1, 1, //
		0, 0, 0, 0, //
		1, 1, 1, 1, //
	};
	float mat_result[16] = {
		1, 1, 1, 1, //
		1, 1, 1, 1, //
		1, 1, 1, 1, //
		1, 1, 1, 1, //
	};
	test_add(mat1, mat2, mat_result);
}

// rand generate and check simd use scalar
void matrix_add_case02()
{
	for (int i = 0; i < 10000; ++i) {
		float *a = (float *)(s_mat_arr + 0);
		float *b = (float *)(s_mat_arr + 1);
		float *c = (float *)(s_mat_arr + 2);
		float *expect_c = (float *)(s_mat_arr + 3);

		for (int i = 0; i < 16; ++i) {
			a[i] = (rand() % 20000 - 10000.0f) / 100.0f;
			b[i] = (rand() % 20000 - 10000.0f) / 100.0f;
		}
		memset(expect_c, 0, sizeof(matrix4_t));
		matrix4_add((const matrix4_t *)a, (const matrix4_t *)b,
					(matrix4_t *)expect_c);

#if SIMD_HAS_SSE4
		memset(c, 0, sizeof(matrix4_t));
		simd_sse4_matrix4_add((const matrix4_t *)a, (const matrix4_t *)b,
							  (matrix4_t *)c);
		check_matrix_equal(c, expect_c);
#endif

#if SIMD_HAS_AVX2
		memset(c, 0, sizeof(matrix4_t));
		simd_avx2_matrix4_add((const matrix4_t *)a, (const matrix4_t *)b,
							  (matrix4_t *)c);
		check_matrix_equal(c, expect_c);
#endif

#if SIMD_HAS_NEON
		memset(c, 0, sizeof(matrix4_t));
		simd_neon_matrix4_add((const matrix4_t *)a, (const matrix4_t *)b,
							  (matrix4_t *)c);
		check_matrix_equal(c, expect_c);
#endif
	}
}

// rand generate and check simd use scalar
void matrix_mul_case01()
{
	for (int i = 0; i < 10000; ++i) {
		float *a = (float *)(s_mat_arr + 0);
		float *b = (float *)(s_mat_arr + 1);
		float *c = (float *)(s_mat_arr + 2);
		float *expect_c = (float *)(s_mat_arr + 3);

		for (int i = 0; i < 16; ++i) {
			a[i] = (rand() % 20000 - 10000.0f) / 100.0f;
			b[i] = (rand() % 20000 - 10000.0f) / 100.0f;
		}
		memset(expect_c, 0, sizeof(matrix4_t));
		matrix4_mul((const matrix4_t *)a, (const matrix4_t *)b,
					(matrix4_t *)expect_c);

#if SIMD_HAS_SSE4
		memset(c, 0, sizeof(matrix4_t));
		simd_sse4_matrix4_mul((const matrix4_t *)a, (const matrix4_t *)b,
							  (matrix4_t *)c);
		check_matrix_equal(c, expect_c);
#endif

#if SIMD_HAS_AVX2
		memset(c, 0, sizeof(matrix4_t));
		simd_avx2_matrix4_mul((const matrix4_t *)a, (const matrix4_t *)b,
							  (matrix4_t *)c);
		check_matrix_equal(c, expect_c);
#endif
	}
}

int main()
{
	UNITY_BEGIN();

	RUN_TEST(matrix_add_case01);
	RUN_TEST(matrix_add_case02);
	RUN_TEST(matrix_mul_case01);

	return UNITY_END();
}
