#include <stdio.h>
#include "common/config.h"

#if SIMD_HAS_SSE4
	#include "immintrin.h"
#endif

#if SIMD_HAS_SSE4
void sse4_shuffle()
{
	float v1[4] = { 1.0f, 2.0f, 3.0f, 4.0f };
	float v2[4] = { 4.0f, 5.0f, 6.0f, 7.0f };
	float v[4];

	__m128 a = _mm_load_ps(v1);
	__m128 b = _mm_load_ps(v2);

	// 从 b 中选
	#define imm_z 2
	#define imm_y 1
	// 从 z 中选
	#define imm_x 0
	#define imm_w 3

	// NOTE:
	//   - result: { a[imm_w], a[imm_x], b[imm_y], b[imm_z] }
	//   - 其中的值来自于 _MM_SHUFFLE 的逆序
	//   - imm_z, imm_y 从 b 中取
	//   - imm_x, imm_w 从 a 中取
	__m128 c = _mm_shuffle_ps(a, b, _MM_SHUFFLE(imm_z, imm_y, imm_x, imm_w));

	_mm_store_ps(v, c);

	printf("expect: {%.2f, %.2f, %.2f, %.2f}\n", v1[imm_w], v1[imm_x],
		   v2[imm_y], v2[imm_z]);
	printf("actual: {%.2f, %.2f, %.2f, %.2f}\n", v[0], v[1], v[2], v[3]);
	printf("imm_w=%d, a[imm_w] = %.2f\n", imm_w, v1[imm_w]);
	printf("imm_x=%d, a[imm_x] = %.2f\n", imm_x, v1[imm_x]);
	printf("imm_y=%d, b[imm_y] = %.2f\n", imm_y, v2[imm_y]);
	printf("imm_z=%d, b[imm_z] = %.2f\n", imm_z, v2[imm_z]);

	#undef imm_w
	#undef imm_x
	#undef imm_y
	#undef imm_z
}
#endif

int main()
{
#if SIMD_HAS_SSE4
	sse4_shuffle();
#endif

	return 0;
}
