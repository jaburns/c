#pragma once

structdef(vec2) { f32 x, y; };
structdef(vec3) { f32 x, y, z; };
structdef(vec4) { f32 x, y, z, w; };

structdef(ivec2) { i32 x, y; };
structdef(ivec3) { i32 x, y, z; };
structdef(ivec4) { i32 x, y, z, w; };

structdef(mat2) { vec2 a, b; };
structdef(mat4) { vec4 a, b, c, d; };

#define sincosf(x, sinp, cosp) __sincosf(x, sinp, cosp)

#define VEC2_ZERO ((vec2){0})
#define VEC2_ONE ((vec2){1.0f, 1.0f})
#define VEC2_RIGHT ((vec2){1.0f, 0.0f})
#define VEC2_UP ((vec2){0.0f, 1.0f})
#define VEC2_LEFT ((vec2){-1.0f, 0.0f})
#define VEC2_DOWN ((vec2){0.0f, -1.0f})

#define DBG_VEC2(v) DBG_TYPED("(vec2){ %f , %f }", (v), (v).x, (v).y)

internal vec2 vec2_add(vec2 a, vec2 b);
internal void vec2_add_to(vec2* a, vec2 b);
internal vec2 vec2_sub(vec2 a, vec2 b);
internal void vec2_sub_from(vec2* a, vec2 b);
internal vec2 vec2_min(vec2 a, vec2 b);
internal vec2 vec2_max(vec2 a, vec2 b);
internal vec2 vec2_negate(vec2 a);
internal vec2 vec2_splat(f32 v);
internal vec2 vec2_scale(f32 s, vec2 v);
internal vec2 vec2_scale_add(vec2 a, f32 s, vec2 b);
internal vec2 vec2_mul(vec2 a, vec2 b);
internal vec2 vec2_div(vec2 v, f32 d);
internal vec2 vec2_lerp(vec2 a, vec2 b, f32 t);
internal f32 vec2_dot(vec2 a, vec2 b);
internal f32 vec2_cross(vec2 a, vec2 b);
internal vec2 vec2_perp(vec2 a);
internal vec2 vec2_fract(vec2 a);
internal vec2 vec2_from_ivec2(ivec2 a);
internal f32 vec2_length(vec2 a);
internal f32 vec2_length_sqr(vec2 a);
internal f32 vec2_distance(vec2 a, vec2 b);
internal vec2 vec2_abs(vec2 a);
internal vec2 vec2_sign(vec2 a);
internal vec2 vec2_clamp(vec2 v, vec2 min, vec2 max);
internal vec2 vec2_normalize(vec2 a);
internal vec2 vec2_rotate(vec2 v, f32 radians);
internal vec2 vec2_reflect_and_scale(vec2 v, vec2 normal, f32 norm_scale, f32 tan_scale);

#define IVEC2_ZERO ((ivec2){0})
#define IVEC2_ONE ((ivec2){1, 1})
#define IVEC2_RIGHT ((ivec2){1, 0})
#define IVEC2_UP ((ivec2){0, 1})
#define IVEC2_LEFT ((ivec2){-1, 0})
#define IVEC2_DOWN ((ivec2){0, -1})

#define DBG_IVEC2(v) DBG_TYPED("(ivec2){ %i , %i }", (v), (v).x, (v).y)

internal ivec2 ivec2_add(ivec2 a, ivec2 b);
internal ivec2 ivec2_sub(ivec2 a, ivec2 b);
internal ivec2 ivec2_min(ivec2 a, ivec2 b);
internal ivec2 ivec2_max(ivec2 a, ivec2 b);
internal ivec2 ivec2_negate(ivec2 a);
internal ivec2 ivec2_splat(i32 v);
internal bool ivec2_eq(ivec2 a, ivec2 b);
internal ivec2 ivec2_from_vec2_floor(vec2 a);
internal ivec2 ivec2_from_vec2_ceil(vec2 a);
internal ivec2 ivec2_from_vec2_round(vec2 a);
internal ivec2 ivec2_clamp(ivec2 v, ivec2 min_inclusive, ivec2 max_exclusive);

#define VEC3_ZERO ((vec3){0})
#define VEC3_ONE ((vec3){1.0f, 1.0f, 1.0f})
#define VEC3_RIGHT ((vec3){1.0f, 0.0f, 0.0f})
#define VEC3_UP ((vec3){0.0f, 1.0f, 0.0f})
#define VEC3_BACK ((vec3){0.0f, 0.0f, 1.0f})
#define VEC3_LEFT ((vec3){-1.0f, 0.0f, 0.0f})
#define VEC3_DOWN ((vec3){0.0f, -1.0f, 0.0f})
#define VEC3_FORWARD ((vec3){0.0f, 0.0f, -1.0f})

#define DBG_VEC3(v) DBG_TYPED("(vec3){ %f , %f , %f }", (v), (v).x, (v).y, (v).z)

internal vec3 vec3_from_vec2(vec2 v, f32 z);
internal f32 vec3_dot(vec3 a, vec3 b);
internal vec3 vec3_scale(vec3 v, f32 scale);
internal vec3 vec3_normalize(vec3 v);

#define VEC4_BLACK ((vec4){0.0f, 0.0f, 0.0f, 1.0f})
#define VEC4_RED ((vec4){1.0f, 0.0f, 0.0f, 1.0f})
#define VEC4_GREEN ((vec4){0.0f, 1.0f, 0.0f, 1.0f})
#define VEC4_BLUE ((vec4){0.0f, 0.0f, 1.0f, 1.0f})
#define VEC4_CYAN ((vec4){0.0f, 1.0f, 1.0f, 1.0f})
#define VEC4_MAGENTA ((vec4){1.0f, 0.0f, 1.0f, 1.0f})
#define VEC4_YELLOW ((vec4){1.0f, 1.0f, 0.0f, 1.0f})
#define VEC4_WHITE ((vec4){1.0f, 1.0f, 1.0f, 1.0f})

#define DBG_VEC4(v) DBG_TYPED("(vec4){ %f , %f , %f , %f }", (v), (v).x, (v).y, (v).z, (v).w)

internal vec4 vec4_scale(vec4 v, f32 scale);
internal vec4 vec4_from_vec3(vec3 v, f32 w);

internal void mat2_identity(mat2* dest);
internal void mat2_make_rotation(mat2* dest, f32 radians);
internal void mat2_make_rotation_pi_over_2(mat2* dest);
internal void mat2_make_rotation_pi(mat2* dest);
internal vec2 mat2_mul_vec2(mat2* m, vec2 v);

internal void mat4_identity(mat4* dest);
internal void mat4_mul(mat4* dest, mat4* m1, mat4* m2);
internal void mat4_make_ortho(mat4* dest, f32 left, f32 right, f32 bottom, f32 top, f32 nearZ, f32 farZ);
internal void mat4_apply_scale(mat4* m, vec3 scale);
internal void mat4_make_rotation(mat4* m, f32 angle, vec3 normalized_axis);

internal f32 f32_lerp(f32 a, f32 b, f32 t);
internal f32 f32_fract(f32 a);

internal f32 ease_out_back(f32 x);
internal f32 ease_out_quad(f32 x);
internal f32 ease_in_quad(f32 x);
internal f32 ease_out_cubic(f32 x);
internal f32 ease_in_cubic(f32 x);
internal f32 ease_in_out_quad(f32 x);

internal f32 radians_sub(f32 lhs, f32 rhs);
internal f32 radians_lerp(f32 a, f32 b, f32 t);
internal f32 radians_mod_zero_to_2pi(f32 rads);
internal f32 radians_minimize_unknown_sine(f32 (*f)(f32));

typedef struct {
    bool on_line;
    vec2 pt;
} LineSegPointResult;

typedef struct {
    bool hit;
    vec2 point;
    vec2 normal;
    f32 t;
} LineSegIntersectResult;

internal LineSegPointResult geo_closest_point_on_line_seg(vec2 pt, vec2 line0, vec2 line1);
internal bool geo_rect_overlaps_circle(vec2 min, vec2 max, vec2 circle_center, f32 circle_radius);
internal bool geo_rect_overlaps_rect(vec2 min0, vec2 max0, vec2 min1, vec2 max1);
internal LineSegIntersectResult geo_line_hit_oriented_line(vec2 a0, vec2 a1, vec2 b0, vec2 b1);
internal LineSegIntersectResult geo_line_hit_circle(vec2 p0, vec2 p1, vec2 center, f32 radius);
internal LineSegIntersectResult geo_line_hit_rect(vec2 p0, vec2 p1, vec2 rect_min, vec2 rect_max);
internal bool geo_rect_contains_point(vec2 rect_min, vec2 rect_max, vec2 pt);