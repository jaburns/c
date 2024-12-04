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

typedef simde_float32x2_t f32x2;
typedef simde_float32x4_t f32x4;
typedef simde_float64x1_t f64x1;
typedef simde_float64x2_t f64x2;

#define u64_from_u8x8(x) ((u64)simde_vreinterpret_u64_u8(x))
#define u16x8_from_u8x16 simde_vreinterpretq_u16_u8
#define i32x2_from_f32x2 simde_vcvt_s32_f32
#define f32x2_from_i32x2 simde_vcvt_f32_s32

#define SimdMacro(t, q, x)     simde_v##x##q##t
#define SimdMacroX(t, q, x, y) simde_v##x##q##_##y##t

#define u8x8(x, ...)      SimdMacro(_u8, , x)(__VA_ARGS__)
#define u8x8x(x, y, ...)  SimdMacroX(_u8, , x, y)(__VA_ARGS__)
#define u8x16(x, ...)     SimdMacro(_u8, q, x)(__VA_ARGS__)
#define u8x16x(x, y, ...) SimdMacroX(_u8, q, x, y)(__VA_ARGS__)

#define u16x4(x, ...)     SimdMacro(_u16, , x)(__VA_ARGS__)
#define u16x4x(x, y, ...) SimdMacroX(_u16, , x, y)(__VA_ARGS__)
#define u16x8(x, ...)     SimdMacro(_u16, q, x)(__VA_ARGS__)
#define u16x8x(x, y, ...) SimdMacroX(_u16, q, x, y)(__VA_ARGS__)

#define u32x2(x, ...)     SimdMacro(_u32, , x)(__VA_ARGS__)
#define u32x2x(x, y, ...) SimdMacroX(_u32, , x, y)(__VA_ARGS__)
#define u32x4(x, ...)     SimdMacro(_u32, q, x)(__VA_ARGS__)
#define u32x4x(x, y, ...) SimdMacroX(_u32, q, x, y)(__VA_ARGS__)

#define u64x1(x, ...)     SimdMacro(_u64, , x)(__VA_ARGS__)
#define u64x1x(x, y, ...) SimdMacroX(_u64, , x, y)(__VA_ARGS__)
#define u64x2(x, ...)     SimdMacro(_u64, q, x)(__VA_ARGS__)
#define u64x2x(x, y, ...) SimdMacroX(_u64, q, x, y)(__VA_ARGS__)

#define i8x8(x, ...)      SimdMacro(_s8, , x)(__VA_ARGS__)
#define i8x8x(x, y, ...)  SimdMacroX(_s8, , x, y)(__VA_ARGS__)
#define i8x16(x, ...)     SimdMacro(_s8, q, x)(__VA_ARGS__)
#define i8x16x(x, y, ...) SimdMacroX(_s8, q, x, y)(__VA_ARGS__)

#define i16x4(x, ...)     SimdMacro(_s16, , x)(__VA_ARGS__)
#define i16x4x(x, y, ...) SimdMacroX(_s16, , x, y)(__VA_ARGS__)
#define i16x8(x, ...)     SimdMacro(_s16, q, x)(__VA_ARGS__)
#define i16x8x(x, y, ...) SimdMacroX(_s16, q, x, y)(__VA_ARGS__)

#define i32x2(x, ...)     SimdMacro(_s32, , x)(__VA_ARGS__)
#define i32x2x(x, y, ...) SimdMacroX(_s32, , x, y)(__VA_ARGS__)
#define i32x4(x, ...)     SimdMacro(_s32, q, x)(__VA_ARGS__)
#define i32x4x(x, y, ...) SimdMacroX(_s32, q, x, y)(__VA_ARGS__)

#define i64x1(x, ...)     SimdMacro(_s64, , x)(__VA_ARGS__)
#define i64x1x(x, y, ...) SimdMacroX(_s64, , x, y)(__VA_ARGS__)
#define i64x2(x, ...)     SimdMacro(_s64, q, x)(__VA_ARGS__)
#define i64x2x(x, y, ...) SimdMacroX(_s64, q, x, y)(__VA_ARGS__)

#define f32x2(x, ...)     SimdMacro(_f32, , x)(__VA_ARGS__)
#define f32x2x(x, y, ...) SimdMacroX(_f32, , x, y)(__VA_ARGS__)
#define f32x4(x, ...)     SimdMacro(_f32, q, x)(__VA_ARGS__)
#define f32x4x(x, y, ...) SimdMacroX(_f32, q, x, y)(__VA_ARGS__)
#define f32x4y(x, ...)    SimdMacro(_f32, , x)(__VA_ARGS__)

#define f64x1(x, ...)     SimdMacro(_f64, , x)(__VA_ARGS__)
#define f64x1x(x, y, ...) SimdMacroX(_f64, , x, y)(__VA_ARGS__)
#define f64x2(x, ...)     SimdMacro(_f64, q, x)(__VA_ARGS__)
#define f64x2x(x, y, ...) SimdMacroX(_f64, q, x, y)(__VA_ARGS__)