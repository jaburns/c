#include "inc.h"

// --------------------------------------------------------------------------------------------------------------------

internal vec2 vec2_add(vec2 a, vec2 b) {
    return vec2_from_f32x2(f32x2_add(a.vector, b.vector));
}

internal vec2 vec2_sub(vec2 a, vec2 b) {
    return vec2_from_f32x2(f32x2_sub(a.vector, b.vector));
}

internal vec2 vec2_min(vec2 a, vec2 b) {
    return vec2_from_f32x2(f32x2_min(a.vector, b.vector));
}

internal vec2 vec2_max(vec2 a, vec2 b) {
    return vec2_from_f32x2(f32x2_max(a.vector, b.vector));
}

internal vec2 vec2_negate(vec2 a) {
    return vec2_from_f32x2(f32x2_negate(a.vector));
}

internal vec2 vec2_splat(f32 v) {
    return vec2_from_f32x2(f32x2_splat(v));
}

internal vec2 vec2_scale(f32 s, vec2 v) {
    return vec2_from_f32x2(f32x2_scale(v.vector, s));
}

internal vec2 vec2_scale_add(vec2 a, f32 s, vec2 b) {
    return vec2_from_f32x2(f32x2_scale_add(a.vector, b.vector, s));
}

internal vec2 vec2_mul(vec2 a, vec2 b) {
    return vec2_from_f32x2(f32x2_mul(a.vector, b.vector));
}

internal vec2 vec2_div_scale(vec2 v, f32 d) {
    return vec2_from_f32x2(f32x2_div(v.vector, f32x2_splat(d)));
}

internal vec2 vec2_lerp(vec2 a, vec2 b, f32 t) {
    return vec2_from_f32x2(f32x2_scale_add(a.vector, f32x2_sub(b.vector, a.vector), t));
}

internal f32 vec2_dot(vec2 a, vec2 b) {
    return f32x2_add_across(f32x2_mul(a.vector, b.vector));
}

internal f32 vec2_cross(vec2 a, vec2 b) {
    u32x2 sign_bit = {0, 0x80000000};
    f32x2 prod     = f32x2_mul(a.vector, f32x2_rotate(b.vector));
    f32x2 flipped  = (f32x2)u32x2_xor((u32x2)prod, sign_bit);
    return f32x2_add_across(flipped);
}

internal vec2 vec2_perp(vec2 a) {
    u32x2 sign_bit   = {0x80000000, 0};
    u32x2 bit_result = u32x2_xor((u32x2)f32x2_rotate(a.vector), sign_bit);
    return vec2_from_f32x2((f32x2)bit_result);
}

internal vec2 vec2_fract(vec2 a) {
    return vec2_from_f32x2(f32x2_sub(a.vector, f32x2_floor(a.vector)));
}

internal vec2 vec2_from_ivec2(ivec2 a) {
    return vec2_from_f32x2(f32x2_from_i32x2(a.vector));
}

internal f32 vec2_length(vec2 a) {
    return sqrtf(f32x2_add_across(f32x2_mul(a.vector, a.vector)));
}

internal f32 vec2_length_sqr(vec2 a) {
    return f32x2_add_across(f32x2_mul(a.vector, a.vector));
}

internal f32 vec2_distance(vec2 a, vec2 b) {
    f32x2 delta = f32x2_sub(a.vector, b.vector);
    return sqrtf(f32x2_add_across(f32x2_mul(delta, delta)));
}

internal vec2 vec2_abs(vec2 a) {
    return vec2_from_f32x2(f32x2_abs(a.vector));
}

internal vec2 vec2_sign(vec2 a) {
    u32x2 neg_mask = f32x2_less_than(a.vector, f32x2_splat(0.f));
    return vec2_from_f32x2(f32x2_select(neg_mask, f32x2_splat(-1.f), f32x2_splat(1.f)));
}

internal vec2 vec2_clamp(vec2 v, vec2 min, vec2 max) {
    return vec2_from_f32x2(f32x2_max(min.vector, f32x2_min(max.vector, v.vector)));
}

internal vec2 vec2_normalize(vec2 a) {
    f32 len = sqrtf(f32x2_add_across(f32x2_mul(a.vector, a.vector)));
    return vec2_from_f32x2(f32x2_div(a.vector, f32x2_splat(len)));
}

internal vec2 vec2_normalize_or_zero(vec2 a) {
    f32 len = sqrtf(f32x2_add_across(f32x2_mul(a.vector, a.vector)));
    if (len < .000001f) return VEC2_ZERO;
    return vec2_from_f32x2(f32x2_div(a.vector, f32x2_splat(len)));
}

internal vec2 vec2_rotate(vec2 v, f32 radians) {
    f32 s, c;
    sincosf(radians, &s, &c);
    f32x2 rx = f32x2_mul(v.vector, (f32x2){c, -s});
    f32x2 ry = f32x2_mul(v.vector, (f32x2){s, c});
    return vec2_from_f32x2(f32x2_add_pairs(rx, ry));
}

// --------------------------------------------------------------------------------------------------------------------

internal ivec2 ivec2_add(ivec2 a, ivec2 b) {
    return ivec2_from_i32x2(i32x2_add(a.vector, b.vector));
}

internal ivec2 ivec2_sub(ivec2 a, ivec2 b) {
    return ivec2_from_i32x2(i32x2_sub(a.vector, b.vector));
}

internal i32 ivec2_manhattan(ivec2 a) {
    return i32x2_add_across(i32x2_abs(a.vector));
}

internal ivec2 ivec2_min(ivec2 a, ivec2 b) {
    return ivec2_from_i32x2(i32x2_min(a.vector, b.vector));
}

internal ivec2 ivec2_max(ivec2 a, ivec2 b) {
    return ivec2_from_i32x2(i32x2_max(a.vector, b.vector));
}

internal ivec2 ivec2_negate(ivec2 a) {
    return ivec2_from_i32x2(i32x2_negate(a.vector));
}

internal ivec2 ivec2_splat(i32 v) {
    return ivec2_from_i32x2(i32x2_splat(v));
}

internal bool ivec2_eq(ivec2 a, ivec2 b) {
    return u32x2_min_across((u32x2)i32x2_equal(a.vector, b.vector)) != 0;
}

internal ivec2 ivec2_from_vec2_floor(vec2 a) {
    return ivec2_from_i32x2(i32x2_from_f32x2(f32x2_floor(a.vector)));
}

internal ivec2 ivec2_from_vec2_ceil(vec2 a) {
    return ivec2_from_i32x2(i32x2_from_f32x2(f32x2_ceil(a.vector)));
}

internal ivec2 ivec2_from_vec2_round(vec2 a) {
    return ivec2_from_i32x2(i32x2_from_f32x2(f32x2_round(a.vector)));
}

internal ivec2 ivec2_clamp(ivec2 v, ivec2 min_inclusive, ivec2 max_exclusive) {
    i32x2 max = i32x2_sub(max_exclusive.vector, i32x2_splat(-1));
    return ivec2_from_i32x2(i32x2_max(min_inclusive.vector, i32x2_min(max, v.vector)));
}

// --------------------------------------------------------------------------------------------------------------------

internal vec3a vec3a_from_vec2(vec2 v, f32 z) {
    return vec3a(v.x, v.y, z);
}

internal f32 vec3a_dot(vec3a a, vec3a b) {
    DebugAssert(a.zero == 0.f && b.zero == 0.f);
    return f32x4_add_across(f32x4_mul(a.vector, b.vector));
}

internal vec3a vec3a_scale(vec3a v, f32 scale) {
    return vec3a_from_f32x4(f32x4_scale(v.vector, scale));
}

internal vec3a vec3a_normalize(vec3a a) {
    DebugAssert(a.zero == 0.f);
    f32 len = sqrtf(f32x4_add_across(f32x4_mul(a.vector, a.vector)));
    return vec3a_from_f32x4(f32x4_div(a.vector, f32x4_splat(len)));
}

// --------------------------------------------------------------------------------------------------------------------

internal vec4 vec4_scale(vec4 v, f32 scale) {
    return vec4_from_f32x4(f32x4_scale(v.vector, scale));
}

internal vec4 vec4_from_vec3a(vec3a v, f32 w) {
    v.zero = w;
    return vec4_from_f32x4(v.vector);
}

// --------------------------------------------------------------------------------------------------------------------

internal void mat2_identity(mat2* dest) {
    dest->a.x = 1.f;
    dest->a.y = 0.f;
    dest->b.x = 0.f;
    dest->b.y = 1.f;
}

internal void mat2_make_rotation(mat2* dest, f32 radians) {
    f32 s, c;
    sincosf(radians, &s, &c);
    dest->a.x = c;
    dest->a.y = -s;
    dest->b.x = s;
    dest->b.y = c;
}

internal void mat2_make_rotation_pi_over_2(mat2* dest) {
    dest->a.x = 0.f;
    dest->a.y = -1.f;
    dest->b.x = 1.f;
    dest->b.y = 0.f;
}

internal void mat2_make_rotation_pi(mat2* dest) {
    dest->a.x = -1.f;
    dest->a.y = 0.f;
    dest->b.x = 0.f;
    dest->b.y = -1.f;
}

internal vec2 mat2_mul_vec2(mat2* m, vec2 v) {
    return (vec2){
        .x = v.x * m->a.x + v.y * m->a.y,
        .y = v.x * m->b.x + v.y * m->b.y,
    };
}

// --------------------------------------------------------------------------------------------------------------------

internal void mat4_identity(mat4* dest) {
    *dest     = (mat4){0};
    dest->a.x = 1.0f;
    dest->b.y = 1.0f;
    dest->c.z = 1.0f;
    dest->d.w = 1.0f;
}

internal void mat4_mul(mat4* dest, mat4* m1, mat4* m2) {
    f32x4 l, r0, r1, r2, r3, v0, v1, v2, v3;

    l  = f32x4_load((f32*)&m1->a);
    r0 = f32x4_load((f32*)&m2->a);
    r1 = f32x4_load((f32*)&m2->b);
    r2 = f32x4_load((f32*)&m2->c);
    r3 = f32x4_load((f32*)&m2->d);

    v0 = f32x4_scale(l, f32x4_get_lane(r0, 0));
    v1 = f32x4_scale(l, f32x4_get_lane(r1, 0));
    v2 = f32x4_scale(l, f32x4_get_lane(r2, 0));
    v3 = f32x4_scale(l, f32x4_get_lane(r3, 0));

    l  = f32x4_load((f32*)&m1->b);
    v0 = f32x4_scale_add(v0, l, f32x4_get_lane(r0, 1));
    v1 = f32x4_scale_add(v1, l, f32x4_get_lane(r1, 1));
    v2 = f32x4_scale_add(v2, l, f32x4_get_lane(r2, 1));
    v3 = f32x4_scale_add(v3, l, f32x4_get_lane(r3, 1));

    l  = f32x4_load((f32*)&m1->c);
    v0 = f32x4_scale_add(v0, l, f32x4_get_lane(r0, 2));
    v1 = f32x4_scale_add(v1, l, f32x4_get_lane(r1, 2));
    v2 = f32x4_scale_add(v2, l, f32x4_get_lane(r2, 2));
    v3 = f32x4_scale_add(v3, l, f32x4_get_lane(r3, 2));

    l  = f32x4_load((f32*)&m1->d);
    v0 = f32x4_scale_add(v0, l, f32x4_get_lane(r0, 3));
    v1 = f32x4_scale_add(v1, l, f32x4_get_lane(r1, 3));
    v2 = f32x4_scale_add(v2, l, f32x4_get_lane(r2, 3));
    v3 = f32x4_scale_add(v3, l, f32x4_get_lane(r3, 3));

    f32x4_store((f32*)&dest->a, v0);
    f32x4_store((f32*)&dest->b, v1);
    f32x4_store((f32*)&dest->c, v2);
    f32x4_store((f32*)&dest->d, v3);
}

internal void mat4_make_ortho(mat4* dest, f32 left, f32 right, f32 bottom, f32 top, f32 nearZ, f32 farZ) {
    f32 rl, tb, fn;

    *dest = (mat4){0};

    rl = 1.0f / (right - left);
    tb = 1.0f / (top - bottom);
    fn = -1.0f / (farZ - nearZ);

    dest->a.x = 2.0f * rl;
    dest->b.y = 2.0f * tb;
    dest->c.z = 2.0f * fn;
    dest->d.x = -(right + left) * rl;
    dest->d.y = -(top + bottom) * tb;
    dest->d.z = (farZ + nearZ) * fn;
    dest->d.w = 1.0f;
}

internal void mat4_apply_scale(mat4* m, vec3a scale) {
    m->a = vec4_scale(m->a, scale.x);
    m->b = vec4_scale(m->b, scale.y);
    m->c = vec4_scale(m->c, scale.z);
}

internal void mat4_make_rotation(mat4* m, f32 angle, vec3a normalized_axis) {
    f32   c  = cosf(angle);
    vec3a v  = vec3a_scale(normalized_axis, 1.0f - c);
    vec3a vs = vec3a_scale(normalized_axis, sinf(angle));
    m->a     = vec4_from_vec3a(vec3a_scale(normalized_axis, v.x), 0.0f);
    m->b     = vec4_from_vec3a(vec3a_scale(normalized_axis, v.y), 0.0f);
    m->c     = vec4_from_vec3a(vec3a_scale(normalized_axis, v.z), 0.0f);

    m->a.x += c;
    m->b.x -= vs.z;
    m->c.x += vs.y;
    m->a.y += vs.z;
    m->b.y += c;
    m->c.y -= vs.x;
    m->a.z -= vs.y;
    m->b.z += vs.x;
    m->c.z += c;

    m->a.w = m->b.w = m->c.w = m->d.x = m->d.y = m->d.z = 0.0f;
    m->d.w                                              = 1.0f;
}

// --------------------------------------------------------------------------------------------------------------------

internal f32 f32_lerp(f32 a, f32 b, f32 t) {
    return a + t * (b - a);
}

internal f32 f32_fract(f32 a) {
    return a - floorf(a);
}

internal f32 ease_out_back(f32 x) {
    f32 c1 = 1.70158f;
    f32 c3 = c1 + 1.f;
    f32 z  = x - 1.f;
    return 1.f + c3 * z * z * z + c1 * z * z;
}

internal f32 ease_out_quad(f32 x) {
    f32 u = 1.f - x;
    return 1.f - u * u;
}

internal f32 ease_in_quad(f32 x) {
    return x * x;
}
internal f32 ease_out_cubic(f32 x) {
    f32 u = 1.f - x;
    return 1.f - u * u * u;
}

internal f32 ease_in_cubic(f32 x) {
    return x * x * x;
}

internal f32 ease_in_out_quad(f32 x) {
    f32 a = -2.f * x + 2.f;
    return x < .5f ? 2.f * x * x : 1.f - a * a * .5f;
}

internal f32 radians_sub(f32 lhs, f32 rhs) {
    f32 diff = fmodf(lhs - rhs, 2.f * M_PI);

    if (diff > M_PI) {
        diff -= 2.f * M_PI;
    } else if (diff < -M_PI) {
        diff += 2.f * M_PI;
    }

    return diff;
}

internal f32 radians_lerp(f32 a, f32 b, f32 t) {
    return a + t * radians_sub(b, a);
}

internal f32 radians_mod_zero_to_2pi(f32 rads) {
    rads = fmodf(rads, 2.f * M_PI);
    if (rads < 0) rads += 2.f * M_PI;
    return rads;
}

// given some sinusoidal function f with period 2pi, but with unknown phase/amplitude/dc, return the
// input to that function in the range 0-2pi which corresponds to the minimum output value.
internal f32 radians_minimize_unknown_sine(f32 (*f)(f32)) {
    f32 f0 = f(0);
    f32 f1 = f(M_PI_2);
    f32 f2 = f(M_PI);

    f32 A = 0.5f * f0 - 0.5f * f2;
    f32 B = -0.5f * f0 + f1 - 0.5f * f2;

    f32 min_angle = 3.f * M_PI_2 - atan2f(A, B);
    if (min_angle > 2.f * M_PI) min_angle -= 2.f * M_PI;

    return min_angle;
}

// --------------------------------------------------------------------------------------------------------------------

internal LineSegPointResult geo_closest_point_on_line_seg(vec2 pt, vec2 line0, vec2 line1) {
    LineSegPointResult result = {0};

    vec2 to_pt    = vec2_sub(pt, line0);
    vec2 line_vec = vec2_sub(line1, line0);

    f32 dot     = vec2_dot(to_pt, line_vec);
    f32 len_sqr = vec2_dot(line_vec, line_vec);
    f32 t       = -1.f;

    if (len_sqr != 0.f) t = dot / len_sqr;

    if (t <= 0.f) {
        result.on_line = false;
        result.pt      = line0;
    } else if (t >= 1.f) {
        result.on_line = false;
        result.pt      = line1;
    } else {
        result.on_line = true;
        result.pt      = vec2_add(line0, vec2_scale(t, line_vec));
    }

    return result;
}

internal bool geo_rect_overlaps_circle(vec2 min, vec2 max, vec2 circle_center, f32 circle_radius) {
    vec2 delta = vec2_sub(circle_center, vec2_clamp(circle_center, min, max));
    return vec2_length_sqr(delta) < circle_radius * circle_radius;
}

internal bool geo_rect_overlaps_rect(vec2 min0, vec2 max0, vec2 min1, vec2 max1) {
    return max1.x > min0.x && min1.x < max0.x && max1.y > min0.y && min1.y < max0.y;
}

internal LineSegIntersectResult geo_line_hit_oriented_line(vec2 a0, vec2 a1, vec2 b0, vec2 b1) {
    LineSegIntersectResult result = {0};

    vec2 r   = vec2_sub(a1, a0);
    vec2 s   = vec2_sub(b1, b0);
    vec2 ba  = vec2_sub(b0, a0);
    f32  rxs = vec2_cross(r, s);

    // only include hits of line A against the left side of line B
    if (rxs <= 0.f) return result;

    f32 t = vec2_cross(ba, s) / rxs;
    if (t < 0.f || t > 1.f) return result;

    f32 u = vec2_cross(ba, r) / rxs;
    if (u < 0.f || u > 1.f) return result;

    result.hit    = true;
    result.point  = vec2_add(a0, vec2_scale(t, r));
    result.normal = vec2(-s.y, s.x);
    result.t      = t;
    return result;
}

internal LineSegIntersectResult geo_line_hit_circle(vec2 p0, vec2 p1, vec2 center, f32 radius) {
    LineSegIntersectResult result = {0};

    vec2 d = vec2_sub(p1, p0);
    vec2 f = vec2_sub(p0, center);

    f32 a    = vec2_dot(d, d);
    f32 b    = 2.f * vec2_dot(f, d);
    f32 c    = vec2_dot(f, f) - radius * radius;
    f32 disc = b * b - 4.f * a * c;

    if (disc < 0.f) return result;

    disc   = sqrtf(disc);
    f32 t0 = (-b - disc) / (2.f * a);
    f32 t1 = (-b + disc) / (2.f * a);

    if (t1 < 0.f || t0 > 1.f) return result;

    f32  t     = Clamp01(t0);
    vec2 point = vec2_add(p0, vec2_scale(t, d));

    result.hit    = true;
    result.point  = point;
    result.normal = vec2_normalize(vec2_sub(point, center));
    result.t      = t;
    return result;
}

internal LineSegIntersectResult geo_line_hit_rect(vec2 p0, vec2 p1, vec2 rect_min, vec2 rect_max) {
    LineSegIntersectResult result = {0};
    if (geo_rect_contains_point(rect_min, rect_max, p0)) {
        result.hit    = true;
        result.normal = vec2_normalize(vec2_sub(p0, p1));
        result.point  = p0;
        result.t      = 0.f;
    }

    result = geo_line_hit_oriented_line(p0, p1, rect_min, vec2(rect_min.x, rect_max.y));
    if (result.hit) return result;
    result = geo_line_hit_oriented_line(p0, p1, vec2(rect_min.x, rect_max.y), rect_max);
    if (result.hit) return result;
    result = geo_line_hit_oriented_line(p0, p1, rect_max, vec2(rect_max.x, rect_min.y));
    if (result.hit) return result;
    result = geo_line_hit_oriented_line(p0, p1, vec2(rect_max.x, rect_min.y), rect_min);
    if (result.hit) return result;

    return (LineSegIntersectResult){0};
}

internal bool geo_rect_contains_point(vec2 rect_min, vec2 rect_max, vec2 pt) {
    return pt.x >= rect_min.x &&
           pt.y >= rect_min.y &&
           pt.x <= rect_max.x &&
           pt.y <= rect_max.y;
}
