//! clang -o /tmp/simd_h_gen simd.h.gen.c && /tmp/simd_h_gen > simd.h
//
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
    NONE   = 0,
    SMALL  = 1,
    SIGNED = 2,
    BYTES  = 4,
} Flags;

void render(char** out, char* jbtype, char* jbname, char* armname0, char* armname1, char* q, char* suffix) {
    *out += sprintf(*out, "#define %s_%-16s simde_v%s%s_%s%s\n", jbtype, jbname, armname0, q, armname1, suffix);
}

void gen_ints(char** out, char* type, char* suffix, Flags flags) {
    char* q = flags & SMALL ? "" : "q";

    render(out, type, "load", "ld1", "", q, suffix);
    render(out, type, "store", "st1", "", q, suffix);
    render(out, type, "splat", "dup", "n_", q, suffix);

    render(out, type, "add", "add", "", q, suffix);
    render(out, type, "sub", "sub", "", q, suffix);
    render(out, type, "mul", "mul", "", q, suffix);
    render(out, type, "min", "min", "", q, suffix);
    render(out, type, "max", "max", "", q, suffix);

    render(out, type, "and", "and", "", q, suffix);
    render(out, type, "or", "orr", "", q, suffix);
    render(out, type, "xor", "eor", "", q, suffix);

    render(out, type, "get_lane", "get", "lane_", q, suffix);
    render(out, type, "extract", "ext", "", q, suffix);
    render(out, type, "select", "bsl", "", q, suffix);

    render(out, type, "add_across", "addv", "", q, suffix);
    render(out, type, "max_across", "maxv", "", q, suffix);
    render(out, type, "min_across", "minv", "", q, suffix);

    render(out, type, "equal", "ceq", "", q, suffix);
    render(out, type, "less_than", "clt", "", q, suffix);
    render(out, type, "greater_than", "cgt", "", q, suffix);

    if (flags & SMALL) {
        render(out, type, "widen", "movl", "", q, suffix);
    } else {
        render(out, type, "get_low", "get_low", "", "", suffix);
        render(out, type, "get_high", "get_high", "", "", suffix);

        if ((flags & BYTES) == 0) {
            render(out, type, "shrn", "shrn", "n_", "", suffix);
        }
    }

    if (flags & SIGNED) {
        render(out, type, "abs", "abs", "", q, suffix);
        render(out, type, "negate", "neg", "", q, suffix);
    }
}

int main(void) {
    char* out  = calloc(1, 1 << 20);
    char* out0 = out;

    out += sprintf(out, "#pragma once\n");
    out += sprintf(out, "\n");
    out += sprintf(out, "typedef simde_uint8x8_t  u8x8;\n");
    out += sprintf(out, "typedef simde_uint8x16_t u8x16;\n");
    out += sprintf(out, "typedef simde_uint16x4_t u16x4;\n");
    out += sprintf(out, "typedef simde_uint16x8_t u16x8;\n");
    out += sprintf(out, "typedef simde_uint32x2_t u32x2;\n");
    out += sprintf(out, "typedef simde_uint32x4_t u32x4;\n");
    out += sprintf(out, "typedef simde_uint64x1_t u64x1;\n");
    out += sprintf(out, "typedef simde_uint64x2_t u64x2;\n");
    out += sprintf(out, "\n");
    out += sprintf(out, "typedef simde_int8x8_t  i8x8;\n");
    out += sprintf(out, "typedef simde_int8x16_t i8x16;\n");
    out += sprintf(out, "typedef simde_int16x4_t i16x4;\n");
    out += sprintf(out, "typedef simde_int16x8_t i16x8;\n");
    out += sprintf(out, "typedef simde_int32x2_t i32x2;\n");
    out += sprintf(out, "typedef simde_int32x4_t i32x4;\n");
    out += sprintf(out, "typedef simde_int64x1_t i64x1;\n");
    out += sprintf(out, "typedef simde_int64x2_t i64x2;\n");
    out += sprintf(out, "\n");
    out += sprintf(out, "typedef simde_float16x4_t f16x4;\n");
    out += sprintf(out, "typedef simde_float16x8_t f16x8;\n");
    out += sprintf(out, "typedef simde_float32x2_t f32x2;\n");
    out += sprintf(out, "typedef simde_float32x4_t f32x4;\n");
    out += sprintf(out, "typedef simde_float64x1_t f64x1;\n");
    out += sprintf(out, "typedef simde_float64x2_t f64x2;\n");
    out += sprintf(out, "\n");
    out += sprintf(out, "// --- convert / reinterpret ---\n");
    out += sprintf(out, "\n");
    out += sprintf(out, "#define u64_from_u8x8(x) ((u64)simde_vreinterpret_u64_u8(x))\n");
    out += sprintf(out, "#define i32x2_from_f32x2 simde_vcvt_s32_f32\n");
    out += sprintf(out, "#define f32x2_from_i32x2 simde_vcvt_f32_s32\n");
    out += sprintf(out, "\n");

    out += sprintf(out, "// --- 8-bit ---\n\n");
    gen_ints(&out, "u8x8", "u8", BYTES | SMALL);
    out += sprintf(out, "\n");
    gen_ints(&out, "u8x16", "u8", BYTES);
    out += sprintf(out, "\n");
    gen_ints(&out, "i8x8", "s8", BYTES | SMALL | SIGNED);
    out += sprintf(out, "\n");
    gen_ints(&out, "i8x16", "s8", BYTES | SIGNED);

    out += sprintf(out, "\n// --- 16-bit ---\n\n");
    gen_ints(&out, "u16x4", "u16", SMALL);
    out += sprintf(out, "\n");
    gen_ints(&out, "u16x8", "u16", NONE);
    out += sprintf(out, "\n");
    gen_ints(&out, "i16x4", "s16", SMALL | SIGNED);
    out += sprintf(out, "\n");
    gen_ints(&out, "i16x8", "s16", SIGNED);

    out += sprintf(out, "\n// --- 32-bit ---\n\n");
    gen_ints(&out, "u32x2", "u32", SMALL);
    out += sprintf(out, "\n");
    gen_ints(&out, "u32x4", "u32", NONE);
    out += sprintf(out, "\n");
    gen_ints(&out, "i32x2", "s32", SMALL | SIGNED);
    out += sprintf(out, "\n");
    gen_ints(&out, "i32x4", "s32", SIGNED);

    out += sprintf(out, "\n// --- float 32 ---\n\n");

    out += sprintf(out, "#define f32x2_splat      simde_vdup_n_f32\n");
    out += sprintf(out, "#define f32x2_scale      simde_vmul_n_f32\n");
    out += sprintf(out, "#define f32x2_abs        simde_vabs_f32\n");
    out += sprintf(out, "#define f32x2_add        simde_vadd_f32\n");
    out += sprintf(out, "#define f32x2_sub        simde_vsub_f32\n");
    out += sprintf(out, "#define f32x2_mul        simde_vmul_f32\n");
    out += sprintf(out, "#define f32x2_div        simde_vdiv_f32\n");
    out += sprintf(out, "#define f32x2_min        simde_vmin_f32\n");
    out += sprintf(out, "#define f32x2_max        simde_vmax_f32\n");
    out += sprintf(out, "#define f32x2_negate     simde_vneg_f32\n");
    out += sprintf(out, "#define f32x2_add_across simde_vaddv_f32\n");
    out += sprintf(out, "#define f32x2_floor      simde_vrndm_f32\n");
    out += sprintf(out, "#define f32x2_ceil       simde_vrndp_f32\n");
    out += sprintf(out, "#define f32x2_round      simde_vrndn_f32\n");
    out += sprintf(out, "#define f32x2_add_pairs  simde_vpadd_f32\n");
    out += sprintf(out, "#define f32x2_scale_add  simde_vmla_n_f32\n");
    out += sprintf(out, "#define f32x2_rotate     simde_vrev64_f32\n");
    out += sprintf(out, "#define f32x2_less_than  simde_vclt_f32\n");
    out += sprintf(out, "#define f32x2_select     simde_vbsl_f32\n");
    out += sprintf(out, "#define f32x2_combine    simde_vcombine_f32\n");
    out += sprintf(out, "\n");
    out += sprintf(out, "#define f32x4_splat      simde_vdupq_n_f32\n");
    out += sprintf(out, "#define f32x4_load       simde_vld1q_f32\n");
    out += sprintf(out, "#define f32x4_store      simde_vst1q_f32\n");
    out += sprintf(out, "#define f32x4_get_lane   simde_vgetq_lane_f32\n");
    out += sprintf(out, "#define f32x4_get_low    simde_vget_low_f32\n");
    out += sprintf(out, "#define f32x4_get_high   simde_vget_high_f32\n");
    out += sprintf(out, "#define f32x4_scale      simde_vmulq_n_f32\n");
    out += sprintf(out, "#define f32x4_abs        simde_vabsq_f32\n");
    out += sprintf(out, "#define f32x4_add        simde_vaddq_f32\n");
    out += sprintf(out, "#define f32x4_sub        simde_vsubq_f32\n");
    out += sprintf(out, "#define f32x4_mul        simde_vmulq_f32\n");
    out += sprintf(out, "#define f32x4_div        simde_vdivq_f32\n");
    out += sprintf(out, "#define f32x4_add_across simde_vaddvq_f32\n");
    out += sprintf(out, "#define f32x4_scale_add  simde_vmlaq_n_f32\n");

    printf("%s", out0);

    return 0;
}
