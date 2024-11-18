#include "inc.h"

#define MAX_SHADER_ERROR_LEN 2048

internal void gfx_shader_error(char* title, char* info_log, i32 line_offset, char** opt_shader_linenos) {
    ArenaTemp scratch = scratch_acquire(NULL, 0);

#if DEBUG
    char* out = arena_alloc(scratch.arena, 2 * MAX_SHADER_ERROR_LEN);
    char* out_write = out;

#define Print(...) PrintfBuf(out_write, out, 2 * MAX_SHADER_ERROR_LEN, __VA_ARGS__)

    Str log = str_from_cstr(info_log);
    for (StrSplitIter lines = str_split_iter('\n', log); !lines.done; str_split_iter_next(&lines)) {
        Str trim_line = str_trim(lines.item);
        if (trim_line.count == 0)
            continue;

        i32 i = 0;
        for (StrSplitIter it = str_split_iter(':', trim_line); !it.done; ++i, str_split_iter_next(&it)) {
            if (i == 2) {
                i32 line = atoi(str_to_cstr(scratch.arena, it.item));
                line -= line_offset;
                char* line_name = opt_shader_linenos ? opt_shader_linenos[line - 1] : "";
                Print("%s: ", line_name);
            } else if (i == 3) {
                Str trim_item = str_trim(it.item);
                Print("%.*s", StrPrintfArgs(trim_item));
            } else if (i > 3) {
                Print(":%.*s", StrPrintfArgs(it.item));
            }
        }
        Print("\n");
    }
    *out_write = '\0';

    fprintf(stderr, "\033[1m\033[91m  %s ERROR \033[0m %s", title, out);

    scratch_release(scratch);

#undef Print
#endif  // DEBUG
}

internal void gfx_shader_create_or_update(
    Shader* out_shader, char* shader_source, char** opt_shader_linenos,
    char** uniform_names, bool* shader_has_uniform_name, size_t uniform_total_names
) {
    bool panic_on_error = !DEBUG || !out_shader->glid;

    ArenaTemp scratch = scratch_acquire(NULL, 0);
    u32 program = 0;

    i32 chunks = 5;
    const char* vert_source[] = {
        "#version 330 core\n",
        "precision highp float;\n",
        "#define VERTEX 1\n",
        "#define FRAGMENT 0\n",
        shader_source
    };
    const char* frag_source[] = {
        "#version 330 core\n",
        "precision highp float;\n",
        "#define VERTEX 0\n",
        "#define FRAGMENT 1\n",
        shader_source
    };

    u32 vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_shader, chunks, vert_source, NULL);
    glCompileShader(vert_shader);

    i32 success;

    char* info_log = arena_alloc(scratch.arena, MAX_SHADER_ERROR_LEN);
    glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vert_shader, MAX_SHADER_ERROR_LEN, NULL, info_log);
        gfx_shader_error("VERT", info_log, chunks - 1, opt_shader_linenos);
        goto error_exit;
    }

    u32 frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shader, chunks, frag_source, NULL);
    glCompileShader(frag_shader);

    glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(frag_shader, MAX_SHADER_ERROR_LEN, NULL, info_log);
        gfx_shader_error("FRAG", info_log, chunks - 1, opt_shader_linenos);
        goto error_exit;
    }

    program = glCreateProgram();
    glAttachShader(program, vert_shader);
    glAttachShader(program, frag_shader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, MAX_SHADER_ERROR_LEN, NULL, info_log);
        gfx_shader_error("LINK", info_log, chunks - 1, opt_shader_linenos);
        goto error_exit;
    }

    gfx_shader_destroy(out_shader);

    for (i32 i = 0; i < uniform_total_names; ++i) {
        if (shader_has_uniform_name[i]) {
            *StaticVecPush(out_shader->uniforms) = (UniformNamedLocation){
                .name_id = i,
                .location = glGetUniformLocation(program, uniform_names[i]),
            };
        }
    }

    out_shader->glid = program;
    panic_on_error = false;

error_exit:
    if (panic_on_error)
        exit(EXIT_FAILURE);

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);

    scratch_release(scratch);
}

internal void gfx_shader_destroy(Shader* program) {
    glDeleteProgram(program->glid);
    ZeroStruct(program);
}

internal u32 gfx_shader_uniform(Shader* program, UniformNameId uniform) {
    i32 found_idx;
    SliceBinarySearch(found_idx, program->uniforms, u32, .name_id, uniform);
    if (found_idx < 0) Panic("Couldn't find uniform location for name id");
    return program->uniforms.items[found_idx].location;
}

internal void gfx_mesh_create_2d(Mesh* mesh, vec2* positions, vec2* uvs, size_t vert_count, u32* indices, size_t index_count) {
    ZeroStruct(mesh);

    u32 vbo_position, vbo_uv, vao, ebo;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo_position);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * vert_count, positions, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), NULL);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vbo_uv);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_uv);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * vert_count, uvs, GL_STATIC_DRAW);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), NULL);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * index_count, indices, GL_STATIC_DRAW);

    mesh->vao = vao;
    mesh->elem_count = index_count;
}

internal void gfx_matrix_construct_model_2d(mat4* model, vec2 position, f32 rotation, vec2 scale) {
    mat4_make_rotation(model, rotation, VEC3_BACK);
    mat4_apply_scale(model, vec3_from_vec2(scale, 1.0f));
    model->d.x = position.x;
    model->d.y = position.y;
}

internal void gfx_mesh_draw(Mesh* mesh) {
    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, mesh->elem_count, GL_UNSIGNED_INT, NULL);
}

internal void gfx_mesh_destroy(Mesh* mesh) {
    u32 vbo0, vbo1, ebo;

    glBindVertexArray(mesh->vao);
    glGetVertexAttribiv(0, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, (i32*)&vbo0);
    glGetVertexAttribiv(1, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, (i32*)&vbo1);
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, (i32*)&ebo);

    glDeleteBuffers(1, &vbo0);
    glDeleteBuffers(1, &vbo1);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &mesh->vao);

    ZeroStruct(mesh);
}

internal void gfx_texture_create(Texture* texture, char* path) {
    ZeroStruct(texture);

    i32 channels_in_file;
    u8* data = stbi_load(path, (i32*)&texture->width, (i32*)&texture->height, &channels_in_file, 4);
    if (!data) {
        Panic("Error loading texture: %s", path);
    }

    glGenTextures(1, &texture->glid);
    glBindTexture(GL_TEXTURE_2D, texture->glid);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  // NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);
}

internal void gfx_texture_destroy(Texture* texture) {
    glDeleteTextures(1, &texture->glid);
    ZeroStruct(texture);
}

internal LineRendererBuffers gfx_make_line_renderer_buffers(Arena* arena, vec2* positions, size_t position_count, f32 half_width) {
    size_t vert_count = 2 * position_count;

    LineRendererBuffers result = {
        .vert_count = vert_count,
        .pos = arena_alloc(arena, vert_count * sizeof(vec2)),
        .uv = arena_alloc(arena, vert_count * sizeof(vec2)),
    };

    for (u32 i = 0; i < position_count; ++i) {
        vec2 midpt = positions[i];
        vec2 tan;

        if (i == 0) {
            tan = vec2_perp(vec2_sub(positions[i + 1], midpt));
        } else if (i == position_count - 1) {
            tan = vec2_perp(vec2_sub(midpt, positions[i - 1]));
        } else {
            vec2 a = vec2_normalize(vec2_sub(positions[i + 1], midpt));
            vec2 b = vec2_normalize(vec2_sub(midpt, positions[i - 1]));
            tan = vec2_perp(vec2_scale(.5f, vec2_add(a, b)));
        }

        tan = vec2_scale(half_width, vec2_normalize(tan));

        result.pos[2 * i] = vec2_sub(midpt, tan);
        result.pos[2 * i + 1] = vec2_add(midpt, tan);
        result.uv[2 * i] = VEC2_ZERO;
        result.uv[2 * i + 1] = VEC2_ZERO;
    }

    return result;
}

#if DEBUG

global DebugGeometry* g_debug_geometry;

internal DebugGeometry* gfx_debug_geometry_alloc(Arena* arena, Shader* shader, Mesh* line_mesh) {
    DebugGeometry* geo = arena_alloc(arena, sizeof(DebugGeometry));
    *geo = (DebugGeometry){
        .lines = VecAlloc(DebugLine, arena, Kb(32)),
        .shader = shader,
        .line_mesh = line_mesh,
    };
    return geo;
}

internal void gfx_debug_set_global_geometry(DebugGeometry* geo) {
    g_debug_geometry = geo;
}

internal void gfx_debug_line2d(vec2 a, vec2 b, vec4 color) {
    *VecPush(g_debug_geometry->lines) = (DebugLine){.a = a, .b = b, .color = color};
}

internal void gfx_debug_circle(vec2 a, f32 radius, vec4 color) {
    vec2 from = (vec2){a.x + radius, a.y};
    for (i32 i = 1; i <= 16; ++i) {
        vec2 to = (vec2){
            a.x + radius * cosf(2 * M_PI * i / 16.f),
            a.y + radius * sinf(2 * M_PI * i / 16.f),
        };
        gfx_debug_line2d(from, to, color);
        from = to;
    }
}

internal void gfx_debug_rect(vec2 a, vec2 b, vec4 color) {
    gfx_debug_line2d((vec2){a.x, a.y}, (vec2){a.x, b.y}, color);
    gfx_debug_line2d((vec2){a.x, b.y}, (vec2){b.x, b.y}, color);
    gfx_debug_line2d((vec2){b.x, b.y}, (vec2){b.x, a.y}, color);
    gfx_debug_line2d((vec2){b.x, a.y}, (vec2){a.x, a.y}, color);
}

internal void gfx_debug_draw(mat4* vp, UniformNameId start_pos, UniformNameId end_pos, UniformNameId color, UniformNameId mvp) {
    for (i32 i = 0; i < g_debug_geometry->lines.count; ++i) {
        DebugLine line = g_debug_geometry->lines.items[i];
        glDisable(GL_DEPTH_TEST);
        glUseProgram(g_debug_geometry->shader->glid);
        glUniform2fv(gfx_shader_uniform(g_debug_geometry->shader, start_pos), 1, (f32*)&line.a);
        glUniform2fv(gfx_shader_uniform(g_debug_geometry->shader, end_pos), 1, (f32*)&line.b);
        glUniform4fv(gfx_shader_uniform(g_debug_geometry->shader, color), 1, (f32*)&line.color);
        glUniformMatrix4fv(gfx_shader_uniform(g_debug_geometry->shader, mvp), 1, false, (f32*)vp);
        glBindVertexArray(g_debug_geometry->line_mesh->vao);
        glDrawElements(GL_LINES, g_debug_geometry->line_mesh->elem_count, GL_UNSIGNED_INT, NULL);
    }
    g_debug_geometry->lines.count = 0;
}

#endif  // DEBUG