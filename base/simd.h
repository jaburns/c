#pragma once

typedef simde_uint8x8_t  u8x8;
typedef simde_uint8x16_t u8x16;
typedef simde_uint16x4_t u16x4;
typedef simde_uint16x8_t u16x8;
typedef simde_uint32x2_t u32x2;
typedef simde_uint32x4_t u32x4;
typedef simde_uint64x1_t u64x1;
typedef simde_uint64x2_t u64x2;

typedef simde_int8x8_t  i8x8;
typedef simde_int8x16_t i8x16;
typedef simde_int16x4_t i16x4;
typedef simde_int16x8_t i16x8;
typedef simde_int32x2_t i32x2;
typedef simde_int32x4_t i32x4;
typedef simde_int64x1_t i64x1;
typedef simde_int64x2_t i64x2;

typedef simde_float16x4_t f16x4;
typedef simde_float16x8_t f16x8;
typedef simde_float32x2_t f32x2;
typedef simde_float32x4_t f32x4;
typedef simde_float64x1_t f64x1;
typedef simde_float64x2_t f64x2;

#define u8x8_load simde_vld1_u8

#define u8x8_splat    simde_vdup_n_u8
#define u8x8_load     simde_vld1_u8
#define u8x8_store    simde_vst1_u8
#define u8x8_equal    simde_vceq_u8
#define u8x8_and      simde_vand_u8
#define u8x8_sum      simde_vaddv_u8
#define u8x8_widen    simde_vmovl_u8
#define u8x8_get_lane simde_vget_lane_u8

#define u8x16_splat     simde_vdupq_n_u8
#define u8x16_load      simde_vld1q_u8
#define u8x16_store     simde_vst1q_u8
#define u8x16_equal     simde_vceqq_u8
#define u8x16_and       simde_vandq_u8
#define u8x16_or_across simde_vorrvq_u8
#define u8x16_sum       simde_vaddvq_u8

#define u16x4_widen      simde_vmovl_u16
#define u16x4_mul        simde_vmul_u16
#define u16x4_add_across simde_vaddv_u16

#define u16x8_get_low  simde_vget_low_u16
#define u16x8_get_high simde_vget_high_u16

#define i32x2_splat      simde_vdup_n_s32
#define i32x2_equal      simde_vceq_s32
#define i32x2_get_lane   simde_vget_lane_s32
#define i32x2_abs        simde_vabs_s32
#define i32x2_add        simde_vadd_s32
#define i32x2_sub        simde_vsub_s32
#define i32x2_min        simde_vmin_s32
#define i32x2_max        simde_vmax_s32
#define i32x2_negate     simde_vneg_s32
#define i32x2_add_across simde_vaddv_s32
#define i32x2_from_f32x2 simde_vcvt_s32_f32

#define u32x2_xor        simde_veor_u32
#define u32x2_min_across simde_vminv_u32

#define i32x4_load       simde_vld1q_s32
#define i32x4_sub        simde_vsubq_s32
#define i32x4_abs        simde_vabsq_s32
#define i32x4_add_across simde_vaddvq_s32

#define u32x4_load       simde_vld1q_u32
#define u32x4_mul        simde_vmulq_u32
#define u32x4_add_across simde_vaddvq_u32
#define u32x4_max_across simde_vmaxvq_u32

#define f32x2_splat      simde_vdup_n_f32
#define f32x2_scale      simde_vmul_n_f32
#define f32x2_abs        simde_vabs_f32
#define f32x2_add        simde_vadd_f32
#define f32x2_sub        simde_vsub_f32
#define f32x2_mul        simde_vmul_f32
#define f32x2_div        simde_vdiv_f32
#define f32x2_min        simde_vmin_f32
#define f32x2_max        simde_vmax_f32
#define f32x2_negate     simde_vneg_f32
#define f32x2_add_across simde_vaddv_f32
#define f32x2_floor      simde_vrndm_f32
#define f32x2_ceil       simde_vrndp_f32
#define f32x2_round      simde_vrndn_f32
#define f32x2_add_pairs  simde_vpadd_f32
#define f32x2_scale_add  simde_vmla_n_f32
#define f32x2_rotate     simde_vrev64_f32
#define f32x2_less_than  simde_vclt_f32
#define f32x2_select     simde_vbsl_f32
#define f32x2_from_i32x2 simde_vcvt_f32_s32
#define f32x2_combine    simde_vcombine_f32

#define f32x4_splat      simde_vdupq_n_f32
#define f32x4_load       simde_vld1q_f32
#define f32x4_store      simde_vst1q_f32
#define f32x4_get_lane   simde_vgetq_lane_f32
#define f32x4_get_low    simde_vget_low_f32
#define f32x4_get_high   simde_vget_high_f32
#define f32x4_scale      simde_vmulq_n_f32
#define f32x4_abs        simde_vabsq_f32
#define f32x4_add        simde_vaddq_f32
#define f32x4_sub        simde_vsubq_f32
#define f32x4_mul        simde_vmulq_f32
#define f32x4_div        simde_vdivq_f32
#define f32x4_add_across simde_vaddvq_f32
#define f32x4_scale_add  simde_vmlaq_n_f32