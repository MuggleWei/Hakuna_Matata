#include "matrix_neon.h"
#include <arm_neon.h>

void simd_neon_matrix4_add(const matrix4_t *mat1, const matrix4_t *mat2,
						   matrix4_t *mat)
{
	float32x4_t a0 = vld1q_f32((const float *)mat1 + 0);
	float32x4_t a1 = vld1q_f32((const float *)mat1 + 4);
	float32x4_t a2 = vld1q_f32((const float *)mat1 + 8);
	float32x4_t a3 = vld1q_f32((const float *)mat1 + 12);

	float32x4_t b0 = vld1q_f32((const float *)mat2 + 0);
	float32x4_t b1 = vld1q_f32((const float *)mat2 + 4);
	float32x4_t b2 = vld1q_f32((const float *)mat2 + 8);
	float32x4_t b3 = vld1q_f32((const float *)mat2 + 12);

	vst1q_f32((float *)mat + 0, vaddq_f32(a0, b0));
	vst1q_f32((float *)mat + 4, vaddq_f32(a1, b1));
	vst1q_f32((float *)mat + 8, vaddq_f32(a2, b2));
	vst1q_f32((float *)mat + 12, vaddq_f32(a3, b3));
}

void simd_neon_matrix4_mul(const matrix4_t *mat1, const matrix4_t *mat2,
						   matrix4_t *mat)
{
	float32x4_t a0 = vld1q_f32((const float *)mat1 + 0);
	float32x4_t a1 = vld1q_f32((const float *)mat1 + 4);
	float32x4_t a2 = vld1q_f32((const float *)mat1 + 8);
	float32x4_t a3 = vld1q_f32((const float *)mat1 + 12);

	float32x4_t b0 = vld1q_f32((const float *)mat2 + 0);
	float32x4_t b1 = vld1q_f32((const float *)mat2 + 4);
	float32x4_t b2 = vld1q_f32((const float *)mat2 + 8);
	float32x4_t b3 = vld1q_f32((const float *)mat2 + 12);

	float32x4_t c0 = vdupq_n_f32(0.0f);
	float32x4_t c1 = vdupq_n_f32(0.0f);
	float32x4_t c2 = vdupq_n_f32(0.0f);
	float32x4_t c3 = vdupq_n_f32(0.0f);

#if !INCREASE_SIMD_ILP 

	c0 = vmlaq_n_f32(c0, b0, vgetq_lane_f32(a0, 0));
	c0 = vmlaq_n_f32(c0, b1, vgetq_lane_f32(a0, 1));
	c0 = vmlaq_n_f32(c0, b2, vgetq_lane_f32(a0, 2));
	c0 = vmlaq_n_f32(c0, b3, vgetq_lane_f32(a0, 3));

	c1 = vmlaq_n_f32(c0, b0, vgetq_lane_f32(a1, 0));
	c1 = vmlaq_n_f32(c0, b1, vgetq_lane_f32(a1, 1));
	c1 = vmlaq_n_f32(c0, b2, vgetq_lane_f32(a1, 2));
	c1 = vmlaq_n_f32(c0, b3, vgetq_lane_f32(a1, 3));

	c2 = vmlaq_n_f32(c0, b0, vgetq_lane_f32(a2, 0));
	c2 = vmlaq_n_f32(c0, b1, vgetq_lane_f32(a2, 1));
	c2 = vmlaq_n_f32(c0, b2, vgetq_lane_f32(a2, 2));
	c2 = vmlaq_n_f32(c0, b3, vgetq_lane_f32(a2, 3));

	c3 = vmlaq_n_f32(c0, b0, vgetq_lane_f32(a3, 0));
	c3 = vmlaq_n_f32(c0, b1, vgetq_lane_f32(a3, 1));
	c3 = vmlaq_n_f32(c0, b2, vgetq_lane_f32(a3, 2));
	c3 = vmlaq_n_f32(c0, b3, vgetq_lane_f32(a3, 3));

#else

	c0 = vmlaq_n_f32(c0, b0, vgetq_lane_f32(a0, 0));
	c1 = vmlaq_n_f32(c0, b0, vgetq_lane_f32(a1, 0));
	c2 = vmlaq_n_f32(c0, b0, vgetq_lane_f32(a2, 0));
	c3 = vmlaq_n_f32(c0, b0, vgetq_lane_f32(a3, 0));

	c0 = vmlaq_n_f32(c0, b1, vgetq_lane_f32(a0, 1));
	c1 = vmlaq_n_f32(c0, b1, vgetq_lane_f32(a1, 1));
	c2 = vmlaq_n_f32(c0, b1, vgetq_lane_f32(a2, 1));
	c3 = vmlaq_n_f32(c0, b1, vgetq_lane_f32(a3, 1));

	c0 = vmlaq_n_f32(c0, b2, vgetq_lane_f32(a0, 2));
	c1 = vmlaq_n_f32(c0, b2, vgetq_lane_f32(a1, 2));
	c2 = vmlaq_n_f32(c0, b2, vgetq_lane_f32(a2, 2));
	c3 = vmlaq_n_f32(c0, b2, vgetq_lane_f32(a3, 2));

	c0 = vmlaq_n_f32(c0, b3, vgetq_lane_f32(a0, 3));
	c1 = vmlaq_n_f32(c0, b3, vgetq_lane_f32(a1, 3));
	c2 = vmlaq_n_f32(c0, b3, vgetq_lane_f32(a2, 3));
	c3 = vmlaq_n_f32(c0, b3, vgetq_lane_f32(a3, 3));

#endif

	vst1q_f32((float *)mat + 0, c0);
	vst1q_f32((float *)mat + 4, c1);
	vst1q_f32((float *)mat + 8, c2);
	vst1q_f32((float *)mat + 12, c3);
}
