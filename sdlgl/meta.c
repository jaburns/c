#include "meta.h"

// --------------------------------------------------------------------------------------------------------------------

internal void resolve_shader_inner(char** shader_sources, char** shader_log_lines, char* file_path, u32* total_lines) {
    ArenaTemp scratch = scratch_acquire(NULL, 0);

    size_t read_len;
    char*  cfile = read_file(scratch.arena, file_path, &read_len);
    Str    file  = (Str){.items = cfile, .count = read_len};
    char   path_buf[256];
    i32    line = 1;

    foreach (StrSplitIter, lines_it, '\n', file) {
        Str trim_line = str_trim(lines_it.item);
        if (str_starts_with_cstr("#include", trim_line)) {
            Str path = str_before_first_index('"', str_after_first_index('"', trim_line));
            snprintf(path_buf, 256, "assets/shaders/%.*s", StrPrintfArgs(path));
            resolve_shader_inner(shader_sources, shader_log_lines, path_buf, total_lines);
        } else {
            *shader_sources   += sprintf(*shader_sources, "\"%.*s\\n\"\n", StrPrintfArgs(trim_line));
            *shader_log_lines += sprintf(*shader_log_lines, "\"%s:%i:0\",\n", file_path, line);
            ++*total_lines;
        }

        line++;
    }

    scratch_release(scratch);
}

internal void resolve_shader(char* name, char** shader_sources, char** shader_log_lines, char* file_path) {
    ArenaTemp scratch = scratch_acquire(NULL, 0);

    char* shader_log_temp   = arena_alloc_nz(scratch.arena, Mb(1));
    char* shader_log_temp_0 = shader_log_temp;

    u32 total_lines  = 1;
    *shader_sources += sprintf(*shader_sources, "readonly_global char* SHADER_SOURCE_%s =\n", name);
    resolve_shader_inner(shader_sources, &shader_log_temp, file_path, &total_lines);

    *shader_log_lines += sprintf(*shader_log_lines, "readonly_global char* SHADER_LINENO_%s[%u] = {\n%s", name, total_lines, shader_log_temp_0);
    *shader_sources   += sprintf(*shader_sources, ";\n\n");
    *shader_log_lines += sprintf(*shader_log_lines, "\"\"};\n\n");

    scratch_release(scratch);
}

// --------------------------------------------------------------------------------------------------------------------

#define MAX_PATH_LENGTH 256
#define MAX_UNIFORMS    100
#define MAX_ENTRIES     100

typedef struct {
    char name[MAX_PATH_LENGTH];
    char path[MAX_PATH_LENGTH];
} AssetEntry;

internal void to_enum_name(char* filename) {
    char* dot = strrchr(filename, '.');
    if (dot) {
        *dot = '\0';
    }
    while (*filename) {
        if (*filename == '.' || *filename == '-' || *filename == ' ') {
            *filename = '_';
        } else {
            *filename = toupper(*filename);
        }
        filename++;
    }
}

internal i32 generate_enum_entries(char* directory, char* extension, AssetEntry* entries, i32 max_entries) {
    DIR* dir = opendir(directory);
    if (!dir) Panic("Failed to open directory");

    struct dirent* entry;
    i32            count = 0;
    while ((entry = readdir(dir)) != NULL && count < max_entries) {
        if (strstr(entry->d_name, ".inc.glsl")) {
            continue;
        }
        if (strstr(entry->d_name, extension)) {
            strncpy(entries[count].name, entry->d_name, MAX_PATH_LENGTH);
            to_enum_name(entries[count].name);
            snprintf(entries[count].path, MAX_PATH_LENGTH, "%s/%s", directory, entry->d_name);
            count++;
        }
    }

    closedir(dir);
    return count;
}

internal i32 extract_uniforms_from_shader(char* file_path, char uniforms[MAX_UNIFORMS][MAX_PATH_LENGTH]) {
    FILE* f = fopen(file_path, "r");
    if (!f) Panic("Failed to open shader file");

    i32  count = 0;
    char line[256];
    while (fgets(line, sizeof(line), f) && count < MAX_UNIFORMS) {
        if (strstr(line, "uniform")) {
            char* name_start = strrchr(line, ' ');
            if (name_start) {
                name_start++;
                char* semicolon = strchr(name_start, ';');
                if (semicolon) {
                    *semicolon = '\0';
                }
                strncpy(uniforms[count], name_start, MAX_PATH_LENGTH);
                count++;
            }
        }
    }

    fclose(f);
    return count;
}

// --------------------------------------------------------------------------------------------------------------------

internal void sdlgl_meta_write_assets_header(char* path) {
    ArenaTemp scratch = scratch_acquire(NULL, 0);

    char* shader_sources     = arena_alloc_nz(scratch.arena, Mb(1));
    char* shader_log_lines   = arena_alloc_nz(scratch.arena, Mb(1));
    char* shader_sources_0   = shader_sources;
    char* shader_log_lines_0 = shader_log_lines;

    AssetEntry audio[MAX_ENTRIES];
    AssetEntry textures[MAX_ENTRIES];
    AssetEntry shaders[MAX_ENTRIES];

    i32 audio_count   = generate_enum_entries("assets/audio", ".ogg", audio, MAX_ENTRIES);
    i32 texture_count = generate_enum_entries("assets/textures", ".png", textures, MAX_ENTRIES);
    i32 shader_count  = generate_enum_entries("assets/shaders", ".glsl", shaders, MAX_ENTRIES);

    char uniform_names[MAX_UNIFORMS][MAX_PATH_LENGTH];
    i32  uniform_name_count = 0;

    bool shader_uniform_map[MAX_ENTRIES][MAX_UNIFORMS] = {false};

    for (i32 i = 0; i < shader_count; i++) {
        resolve_shader(shaders[i].name, &shader_sources, &shader_log_lines, shaders[i].path);

        char shader_uniforms[MAX_UNIFORMS][MAX_PATH_LENGTH];
        i32  shader_uniform_count = extract_uniforms_from_shader(shaders[i].path, shader_uniforms);

        for (i32 j = 0; j < shader_uniform_count; j++) {
            i32 uniform_index = -1;
            for (i32 k = 0; k < uniform_name_count; k++) {
                if (strcmp(uniform_names[k], shader_uniforms[j]) == 0) {
                    uniform_index = k;
                    break;
                }
            }
            if (uniform_index == -1 && uniform_name_count < MAX_UNIFORMS) {
                strncpy(uniform_names[uniform_name_count], shader_uniforms[j], MAX_PATH_LENGTH);
                uniform_index = uniform_name_count;
                uniform_name_count++;
            }
            if (uniform_index != -1) {
                shader_uniform_map[i][uniform_index] = true;
            }
        }
    }

    FILE* f = fopen(path, "w");
    if (!f) Panic("Failed to create header file");

    fprintf(f, "#pragma once\n\n");

    fprintf(f, "%s#if DEBUG\n\n%s#endif // DEBUG\n\n", shader_sources_0, shader_log_lines_0);

    fprintf(f, "#define AUDIOCLIP_COUNT %d\n", audio_count + 1);
    for (i32 i = 0; i < audio_count; i++) {
        fprintf(f, "#define AUDIOCLIP_IDX_%s %d\n", audio[i].name, i + 1);
    }
    fprintf(f, "readonly_global char* AUDIOCLIP_PATHS[AUDIOCLIP_COUNT] = {\n    NULL,\n");
    for (i32 i = 0; i < audio_count; i++) {
        fprintf(f, "    \"%s\",\n", audio[i].path);
    }
    fprintf(f, "};\n\n");

    fprintf(f, "#define TEXTURE_COUNT %d\n", texture_count + 1);
    for (i32 i = 0; i < texture_count; i++) {
        fprintf(f, "#define TEXTURE_IDX_%s %d\n", textures[i].name, i + 1);
    }
    fprintf(f, "readonly_global char* TEXTURE_PATHS[TEXTURE_COUNT] = {\n    NULL,\n");
    for (i32 i = 0; i < texture_count; i++) {
        fprintf(f, "    \"%s\",\n", textures[i].path);
    }
    fprintf(f, "};\n\n");

    fprintf(f, "#define SHADER_COUNT %d\n", shader_count + 1);
    for (i32 i = 0; i < shader_count; i++) {
        fprintf(f, "#define SHADER_IDX_%s %d\n", shaders[i].name, i + 1);
    }
    fprintf(f, "readonly_global char** SHADER_SOURCES[SHADER_COUNT] = {\n    NULL,\n");
    for (i32 i = 0; i < shader_count; i++) {
        fprintf(f, "    &SHADER_SOURCE_%s,\n", shaders[i].name);
    }
    fprintf(f, "};\n");
    fprintf(f, "#if DEBUG\n");
    fprintf(f, "readonly_global char** SHADER_LINENOS[SHADER_COUNT] = {\n    NULL,\n");
    for (i32 i = 0; i < shader_count; i++) {
        fprintf(f, "    (char**)SHADER_LINENO_%s,\n", shaders[i].name);
    }
    fprintf(f, "};\n");
    fprintf(f, "#endif // DEBUG\n\n");

    fprintf(f, "#define UNIFORM_TOTAL_NAMES %d\n\n", uniform_name_count);
    fprintf(f, "readonly_global char* UNIFORM_NAMES[UNIFORM_TOTAL_NAMES] = {\n");
    for (i32 i = 0; i < uniform_name_count; i++) {
        fprintf(f, "    \"%s\",\n", uniform_names[i]);
    }
    fprintf(f, "};\n\n");

    fprintf(f, "readonly_global bool SHADER_HAS_UNIFORM_NAME[SHADER_COUNT][UNIFORM_TOTAL_NAMES] = {\n");
    fprintf(f, "    {");
    for (i32 i = 0; i < uniform_name_count; i++) {
        fprintf(f, "false, ");
    }
    fprintf(f, "},\n");
    for (i32 i = 0; i < shader_count; i++) {
        fprintf(f, "    {");
        for (i32 j = 0; j < uniform_name_count; j++) {
            fprintf(f, "%s, ", shader_uniform_map[i][j] ? "true" : "false");
        }
        fprintf(f, "},\n");
    }
    fprintf(f, "};\n\n");

    for (i32 i = 0; i < uniform_name_count; i++) {
        char* name = uniform_names[i];
        while (*name) {
            *name = toupper(*name);
            name++;
        }
        fprintf(f, "#define UNIFORM_%s %d\n", uniform_names[i], i);
    }

    fclose(f);
    scratch_release(scratch);
}