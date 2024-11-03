#pragma once

typedef u32 UniformNameId;

typedef struct {
    UniformNameId name_id;
    u32 location;
} UniformNamedLocation;

typedef struct {
    u32 glid;
    DARRAY(UniformNamedLocation, 64) uniforms;
} GfxShader;

typedef struct {
    u32 glid;
    u32 width;
    u32 height;
} GfxTexture;

typedef struct {
    u32 vao;
    u32 elem_count;
} GfxMesh;

typedef struct {
    size_t vert_count;
    vec2* pos;
    vec2* uv;
} LineRendererBuffers;

internal void gfx_shader_create_or_update(
    GfxShader* out_shader, char* shader_source, char** opt_shader_linenos,
    char** uniform_names, bool* shader_has_uniform_name, size_t uniform_total_names
);
internal void gfx_shader_destroy(GfxShader* shader);
internal u32 gfx_shader_uniform(GfxShader* program, UniformNameId uniform);

internal void gfx_mesh_create_2d(GfxMesh* mesh, vec2* positions, vec2* uvs, size_t vert_count, u32* indices, size_t index_count);
internal void gfx_mesh_destroy(GfxMesh* mesh);
internal void gfx_mesh_draw(GfxMesh* mesh);

internal void gfx_matrix_construct_model_2d(mat4* model, vec2 position, f32 rotation, vec2 scale);

internal void gfx_texture_create(GfxTexture* texture, char* path);
internal void gfx_texture_destroy(GfxTexture* texture);

internal LineRendererBuffers gfx_make_line_renderer_buffers(Arena* arena, vec2* positions, size_t position_count, f32 half_width);

internal void gfx_debug_init(GfxShader* shader, GfxMesh* mesh) DEBUG_FN;
internal void gfx_debug_line2d(vec2 a, vec2 b, vec4 color) DEBUG_FN;
internal void gfx_debug_circle(vec2 a, f32 radius, vec4 color) DEBUG_FN;
internal void gfx_debug_rect(vec2 a, vec2 b, vec4 color) DEBUG_FN;
internal void gfx_debug_draw(mat4* vp, u32 start_pos, u32 end_pos, u32 color, u32 mvp) DEBUG_FN;
