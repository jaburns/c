#include "inc.h"

// --------------------------------------------------------------------------------------------------------------------

internal vec2 vec2_add(vec2 a, vec2 b) {
    return (vec2){
        .x = a.x + b.x,
        .y = a.y + b.y,
    };
}

internal void vec2_add_to(vec2* a, vec2 b) {
    a->x += b.x;
    a->y += b.y;
}

internal vec2 vec2_sub(vec2 a, vec2 b) {
    return (vec2){
        .x = a.x - b.x,
        .y = a.y - b.y,
    };
}

internal void vec2_sub_from(vec2* a, vec2 b) {
    a->x -= b.x;
    a->y -= b.y;
}

internal vec2 vec2_min(vec2 a, vec2 b) {
    return (vec2){
        .x = Min(a.x, b.x),
        .y = Min(a.y, b.y),
    };
}

internal vec2 vec2_max(vec2 a, vec2 b) {
    return (vec2){
        .x = Max(a.x, b.x),
        .y = Max(a.y, b.y),
    };
}

internal vec2 vec2_negate(vec2 a) {
    return (vec2){
        .x = -a.x,
        .y = -a.y,
    };
}

internal vec2 vec2_splat(f32 v) {
    return (vec2){v, v};
}

internal vec2 vec2_scale(f32 s, vec2 v) {
    return (vec2){s * v.x, s * v.y};
}

internal vec2 vec2_scale_add(vec2 a, f32 s, vec2 b) {
    return (vec2){s * b.x + a.x, s * b.y + a.y};
}

internal vec2 vec2_mul(vec2 a, vec2 b) {
    return (vec2){a.x * b.x, a.y * b.y};
}

internal vec2 vec2_div(vec2 v, f32 d) {
    return (vec2){v.x / d, v.y / d};
}

internal vec2 vec2_lerp(vec2 a, vec2 b, f32 t) {
    return (vec2){
        .x = a.x + t * (b.x - a.x),
        .y = a.y + t * (b.y - a.y),
    };
}

internal f32 vec2_dot(vec2 a, vec2 b) {
    return a.x * b.x + a.y * b.y;
}

internal f32 vec2_cross(vec2 a, vec2 b) {
    return a.x * b.y - a.y * b.x;
}

internal vec2 vec2_perp(vec2 a) {
    return (vec2){-a.y, a.x};
}

internal vec2 vec2_fract(vec2 a) {
    return (vec2){
        .x = a.x - floorf(a.x),
        .y = a.y - floorf(a.y),
    };
}

internal vec2 vec2_from_ivec2(ivec2 a) {
    return (vec2){a.x, a.y};
}

internal f32 vec2_length(vec2 a) {
    return sqrtf(a.x * a.x + a.y * a.y);
}

internal f32 vec2_length_sqr(vec2 a) {
    return a.x * a.x + a.y * a.y;
}

internal f32 vec2_distance(vec2 a, vec2 b) {
    f32 dx = a.x - b.x;
    f32 dy = a.y - b.y;
    return sqrtf(dx * dx + dy * dy);
}

internal vec2 vec2_abs(vec2 a) {
    return (vec2){fabsf(a.x), fabsf(a.y)};
}

internal vec2 vec2_sign(vec2 a) {
    return (vec2){Sign(a.x), Sign(a.y)};
}

internal vec2 vec2_clamp(vec2 v, vec2 min, vec2 max) {
    return (vec2){
        Max(min.x, Min(max.x, v.x)),
        Max(min.y, Min(max.y, v.y)),
    };
}

internal vec2 vec2_normalize(vec2 a) {
    f32 inv_len = 1.f / sqrtf(a.x * a.x + a.y * a.y);
    return (vec2){a.x * inv_len, a.y * inv_len};
}

internal vec2 vec2_normalize_or_zero(vec2 a) {
    f32 len = sqrtf(a.x * a.x + a.y * a.y);
    if (len < .000001f) return VEC2_ZERO;
    f32 inv_len = 1.f / len;
    return (vec2){a.x * inv_len, a.y * inv_len};
}

internal vec2 vec2_rotate(vec2 v, f32 radians) {
    f32 s, c;
    sincosf(radians, &s, &c);
    return (vec2){
        v.x * c - v.y * s,
        v.x * s + v.y * c,
    };
}

internal vec2 vec2_reflect_and_scale(vec2 v, vec2 normal, f32 norm_scale, f32 tan_scale) {
    f32 n = vec2_dot(v, normal);
    if (n >= 0.f) return v;
    vec2 tangent = vec2_perp(normal);
    f32  t       = vec2_dot(v, tangent);

    return vec2_add(
        vec2_scale(tan_scale * t, tangent),
        vec2_scale(-norm_scale * n, normal)
    );
}

// --------------------------------------------------------------------------------------------------------------------

internal ivec2 ivec2_add(ivec2 a, ivec2 b) {
    return (ivec2){
        .x = a.x + b.x,
        .y = a.y + b.y,
    };
}

internal ivec2 ivec2_sub(ivec2 a, ivec2 b) {
    return (ivec2){
        .x = a.x - b.x,
        .y = a.y - b.y,
    };
}

internal ivec2 ivec2_min(ivec2 a, ivec2 b) {
    return (ivec2){
        .x = Min(a.x, b.x),
        .y = Min(a.y, b.y),
    };
}

internal ivec2 ivec2_max(ivec2 a, ivec2 b) {
    return (ivec2){
        .x = Max(a.x, b.x),
        .y = Max(a.y, b.y),
    };
}

internal ivec2 ivec2_negate(ivec2 a) {
    return (ivec2){
        .x = -a.x,
        .y = -a.y,
    };
}

internal ivec2 ivec2_splat(i32 v) {
    return (ivec2){v, v};
}

internal bool ivec2_eq(ivec2 a, ivec2 b) {
    return a.x == b.x && a.y == b.y;
}

internal ivec2 ivec2_from_vec2_floor(vec2 a) {
    return (ivec2){floorf(a.x), floorf(a.y)};
}

internal ivec2 ivec2_from_vec2_ceil(vec2 a) {
    return (ivec2){ceilf(a.x), ceilf(a.y)};
}

internal ivec2 ivec2_from_vec2_round(vec2 a) {
    return (ivec2){roundf(a.x), roundf(a.y)};
}

internal ivec2 ivec2_clamp(ivec2 v, ivec2 min_inclusive, ivec2 max_exclusive) {
    return (ivec2){
        .x = v.x < min_inclusive.x ? min_inclusive.x : v.x >= max_exclusive.x ? max_exclusive.x - 1
                                                                              : v.x,
        .y = v.y < min_inclusive.y ? min_inclusive.y : v.y >= max_exclusive.y ? max_exclusive.y - 1
                                                                              : v.y,
    };
}

// --------------------------------------------------------------------------------------------------------------------

internal vec3 vec3_from_vec2(vec2 v, f32 z) {
    return (vec3){v.x, v.y, z};
}

internal f32 vec3_dot(vec3 a, vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

internal vec3 vec3_scale(vec3 v, f32 scale) {
    return (vec3){scale * v.x, scale * v.y, scale * v.z};
}

internal vec3 vec3_normalize(vec3 a) {
    f32 inv_len = 1.f / sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
    return (vec3){a.x * inv_len, a.y * inv_len, a.z * inv_len};
}

// --------------------------------------------------------------------------------------------------------------------

internal vec4 vec4_scale(vec4 v, f32 scale) {
    return (vec4){scale * v.x, scale * v.y, scale * v.z, scale * v.w};
}

internal vec4 vec4_from_vec3(vec3 v, f32 w) {
    return (vec4){v.x, v.y, v.z, w};
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
        v.x * m->a.x + v.y * m->a.y,
        v.x * m->b.x + v.y * m->b.y,
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
    simde_float32x4_t l, r0, r1, r2, r3, v0, v1, v2, v3;

    l  = simde_vld1q_f32((simde_float32*)&m1->a);
    r0 = simde_vld1q_f32((simde_float32*)&m2->a);
    r1 = simde_vld1q_f32((simde_float32*)&m2->b);
    r2 = simde_vld1q_f32((simde_float32*)&m2->c);
    r3 = simde_vld1q_f32((simde_float32*)&m2->d);

    v0 = simde_vmulq_n_f32(l, simde_vgetq_lane_f32(r0, 0));
    v1 = simde_vmulq_n_f32(l, simde_vgetq_lane_f32(r1, 0));
    v2 = simde_vmulq_n_f32(l, simde_vgetq_lane_f32(r2, 0));
    v3 = simde_vmulq_n_f32(l, simde_vgetq_lane_f32(r3, 0));

    l  = simde_vld1q_f32((simde_float32*)&m1->b);
    v0 = simde_vmlaq_n_f32(v0, l, simde_vgetq_lane_f32(r0, 1));
    v1 = simde_vmlaq_n_f32(v1, l, simde_vgetq_lane_f32(r1, 1));
    v2 = simde_vmlaq_n_f32(v2, l, simde_vgetq_lane_f32(r2, 1));
    v3 = simde_vmlaq_n_f32(v3, l, simde_vgetq_lane_f32(r3, 1));

    l  = simde_vld1q_f32((simde_float32*)&m1->c);
    v0 = simde_vmlaq_n_f32(v0, l, simde_vgetq_lane_f32(r0, 2));
    v1 = simde_vmlaq_n_f32(v1, l, simde_vgetq_lane_f32(r1, 2));
    v2 = simde_vmlaq_n_f32(v2, l, simde_vgetq_lane_f32(r2, 2));
    v3 = simde_vmlaq_n_f32(v3, l, simde_vgetq_lane_f32(r3, 2));

    l  = simde_vld1q_f32((simde_float32*)&m1->d);
    v0 = simde_vmlaq_n_f32(v0, l, simde_vgetq_lane_f32(r0, 3));
    v1 = simde_vmlaq_n_f32(v1, l, simde_vgetq_lane_f32(r1, 3));
    v2 = simde_vmlaq_n_f32(v2, l, simde_vgetq_lane_f32(r2, 3));
    v3 = simde_vmlaq_n_f32(v3, l, simde_vgetq_lane_f32(r3, 3));

    simde_vst1q_f32((simde_float32*)&dest->a, v0);
    simde_vst1q_f32((simde_float32*)&dest->b, v1);
    simde_vst1q_f32((simde_float32*)&dest->c, v2);
    simde_vst1q_f32((simde_float32*)&dest->d, v3);
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

internal void mat4_apply_scale(mat4* m, vec3 scale) {
    m->a = vec4_scale(m->a, scale.x);
    m->b = vec4_scale(m->b, scale.y);
    m->c = vec4_scale(m->c, scale.z);
}

internal void mat4_make_rotation(mat4* m, f32 angle, vec3 normalized_axis) {
    f32  c  = cosf(angle);
    vec3 v  = vec3_scale(normalized_axis, 1.0f - c);
    vec3 vs = vec3_scale(normalized_axis, sinf(angle));
    m->a    = vec4_from_vec3(vec3_scale(normalized_axis, v.x), 0.0f);
    m->b    = vec4_from_vec3(vec3_scale(normalized_axis, v.y), 0.0f);
    m->c    = vec4_from_vec3(vec3_scale(normalized_axis, v.z), 0.0f);

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

// Given some sinusoidal function f with period 2pi, but with unknown phase/amplitude/dc, return the
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

    // Only include hits of line A against the left side of line B
    if (rxs <= 0.f) return result;

    f32 t = vec2_cross(ba, s) / rxs;
    if (t < 0.f || t > 1.f) return result;

    f32 u = vec2_cross(ba, r) / rxs;
    if (u < 0.f || u > 1.f) return result;

    result.hit    = true;
    result.point  = vec2_add(a0, vec2_scale(t, r));
    result.normal = (vec2){-s.y, s.x};
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

    result = geo_line_hit_oriented_line(p0, p1, rect_min, (vec2){rect_min.x, rect_max.y});
    if (result.hit) return result;
    result = geo_line_hit_oriented_line(p0, p1, (vec2){rect_min.x, rect_max.y}, rect_max);
    if (result.hit) return result;
    result = geo_line_hit_oriented_line(p0, p1, rect_max, (vec2){rect_max.x, rect_min.y});
    if (result.hit) return result;
    result = geo_line_hit_oriented_line(p0, p1, (vec2){rect_max.x, rect_min.y}, rect_min);
    if (result.hit) return result;

    return (LineSegIntersectResult){0};
}

internal bool geo_rect_contains_point(vec2 rect_min, vec2 rect_max, vec2 pt) {
    return pt.x >= rect_min.x &&
           pt.y >= rect_min.y &&
           pt.x <= rect_max.x &&
           pt.y <= rect_max.y;
}
