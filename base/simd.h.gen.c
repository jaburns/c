#if 0
clang -o /tmp/jbcbin simd.h.gen.c && /tmp/jbcbin > simd.h
exit 0
#endif
//
// This file is directly executable by bash
//
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
    NONE   = 0,
    SMALL  = 1,
    SIGNED = 2,
    BYTES  = 4,
    FLOAT  = 8,
} Flags;

void render(char** out, char* jbtype, char* jbname, char* armname0, char* armname1, char* q, char* suffix) {
    *out += sprintf(*out, "#define %s_%-16s simde_v%s%s_%s%s\n", jbtype, jbname, armname0, q, armname1, suffix);
}

void generate(char** out, char* type, char* suffix, Flags flags) {
    char* q = flags & SMALL ? "" : "q";

    render(out, type, "load", "ld1", "", q, suffix);
    render(out, type, "store", "st1", "", q, suffix);
    render(out, type, "splat", "dup", "n_", q, suffix);

    render(out, type, "add", "add", "", q, suffix);
    render(out, type, "sub", "sub", "", q, suffix);
    render(out, type, "mul", "mul", "", q, suffix);
    render(out, type, "min", "min", "", q, suffix);
    render(out, type, "max", "max", "", q, suffix);

    render(out, type, "add_pairs", "padd", "", q, suffix);
    render(out, type, "reverse64", "rev64", "", q, suffix);

    if (flags & FLOAT) {
        render(out, type, "div", "div", "", q, suffix);
        render(out, type, "scale", "mul", "n_", q, suffix);
        render(out, type, "floor", "rndm", "", q, suffix);
        render(out, type, "ceil", "rndp", "", q, suffix);
        render(out, type, "round", "rndn", "", q, suffix);

        render(out, type, "scale_add", "mla", "n_", q, suffix);
    } else {
        render(out, type, "and", "and", "", q, suffix);
        render(out, type, "or", "orr", "", q, suffix);
        render(out, type, "xor", "eor", "", q, suffix);
    }

    if (flags & BYTES) {
        render(out, type, "swizzle", "qtbl1", "", q, suffix);
    }

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
        if ((flags & SIGNED) == 0) {
            render(out, type, "widen", "movl", "", q, suffix);
        }
        render(out, type, "combine", "combine", "", q, suffix);
    } else {
        render(out, type, "get_low", "get_low", "", "", suffix);
        render(out, type, "get_high", "get_high", "", "", suffix);

        if ((flags & BYTES) == 0 && (flags & FLOAT) == 0) {
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
    out += sprintf(out, "#define u16x8_from_u8x16 simde_vreinterpretq_u16_u8\n");
    out += sprintf(out, "#define u8x16_from_u16x8 simde_vreinterpretq_u8_u16\n");
    out += sprintf(out, "#define i32x2_from_f32x2 simde_vcvt_s32_f32\n");
    out += sprintf(out, "#define f32x2_from_i32x2 simde_vcvt_f32_s32\n");
    out += sprintf(out, "#define i32x4_from_f32x4 simde_vcvtq_s32_f32\n");
    out += sprintf(out, "#define f32x4_from_i32x4 simde_vcvtq_f32_s32\n");
    out += sprintf(out, "\n");

    out += sprintf(out, "// --- 8-bit ---\n\n");
    generate(&out, "u8x8", "u8", BYTES | SMALL);
    out += sprintf(out, "\n");
    generate(&out, "u8x16", "u8", BYTES);
    out += sprintf(out, "\n");
    generate(&out, "i8x8", "s8", BYTES | SMALL | SIGNED);
    out += sprintf(out, "\n");
    generate(&out, "i8x16", "s8", BYTES | SIGNED);

    out += sprintf(out, "\n");
    out += sprintf(out, "#define u8x8_nonzero_lane(x)    (u64_count_leading_zeroes(u64_from_u8x8(u8x8_reverse64(x))) / 8)\n");
    out += sprintf(out, "#define u8x16_nonzero_lane(x)   (u64_count_leading_zeroes(u64_bit_reverse(u64_from_u8x8(u16x8_shrn(u16x8_from_u8x16(x), 4)))) / 4)\n");
    out += sprintf(out, "#define u8x16_shift_lanes(x, n) (u8x16_extract((x), u8x16_splat(0), (n)))\n");

    out += sprintf(out, "\n// --- 16-bit ---\n\n");
    generate(&out, "u16x4", "u16", SMALL);
    out += sprintf(out, "\n");
    generate(&out, "u16x8", "u16", NONE);
    out += sprintf(out, "\n");
    generate(&out, "i16x4", "s16", SMALL | SIGNED);
    out += sprintf(out, "\n");
    generate(&out, "i16x8", "s16", SIGNED);

    out += sprintf(out, "\n// --- 32-bit ---\n\n");
    generate(&out, "u32x2", "u32", SMALL);
    out += sprintf(out, "\n");
    generate(&out, "u32x4", "u32", NONE);
    out += sprintf(out, "\n");
    generate(&out, "i32x2", "s32", SMALL | SIGNED);
    out += sprintf(out, "\n");
    generate(&out, "i32x4", "s32", SIGNED);

    out += sprintf(out, "\n// --- float 32 ---\n\n");
    generate(&out, "f32x2", "f32", SMALL | SIGNED | FLOAT);
    out += sprintf(out, "\n");
    generate(&out, "f32x4", "f32", SIGNED | FLOAT);

    printf("%s", out0);

    return 0;
}
