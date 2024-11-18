#pragma once

typedef u32 UniformNameId;

structdef(UniformNamedLocation) {
    UniformNameId name_id;
    u32 location;
};

structdef(Shader) {
    u32 glid;
    StaticVec(UniformNamedLocation, 64) uniforms;
};

structdef(Texture) {
    u32 glid;
    u32 width;
    u32 height;
};

structdef(Mesh) {
    u32 vao;
    u32 elem_count;
};

structdef(LineRendererBuffers) {
    size_t vert_count;
    vec2* pos;
    vec2* uv;
};

internal void gfx_shader_create_or_update(
    Shader* out_shader, char* shader_source, char** opt_shader_linenos,
    char** uniform_names, bool* shader_has_uniform_name, size_t uniform_total_names
);
internal void gfx_shader_destroy(Shader* shader);
internal u32 gfx_shader_uniform(Shader* program, UniformNameId uniform);

internal void gfx_mesh_create_2d(Mesh* mesh, vec2* positions, vec2* uvs, size_t vert_count, u32* indices, size_t index_count);
internal void gfx_mesh_destroy(Mesh* mesh);
internal void gfx_mesh_draw(Mesh* mesh);

internal void gfx_matrix_construct_model_2d(mat4* model, vec2 position, f32 rotation, vec2 scale);

internal void gfx_texture_create(Texture* texture, char* path);
internal void gfx_texture_destroy(Texture* texture);

internal LineRendererBuffers gfx_make_line_renderer_buffers(Arena* arena, vec2* positions, size_t position_count, f32 half_width);

#if DEBUG

structdef(DebugLine) {
    vec4 color;
    vec2 a;
    vec2 b;
};
DefArrayTypes(DebugLine);

structdef(DebugGeometry) {
    Shader* shader;
    Mesh* line_mesh;
    Vec_DebugLine lines;
};

internal DebugGeometry* gfx_debug_geometry_alloc(Arena* arena, Shader* shader, Mesh* line_mesh);
internal void gfx_debug_set_global_geometry(DebugGeometry* geo);

#endif

internal void gfx_debug_line2d(vec2 a, vec2 b, vec4 color) DEBUG_FN;
internal void gfx_debug_circle(vec2 a, f32 radius, vec4 color) DEBUG_FN;
internal void gfx_debug_rect(vec2 a, vec2 b, vec4 color) DEBUG_FN;
internal void gfx_debug_draw(mat4* vp, u32 start_pos, u32 end_pos, u32 color, u32 mvp) DEBUG_FN;
