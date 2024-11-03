#include "inc.h"

internal void xml_parse(
    char* file_contents,
    size_t file_length,
    void* user_ctx,
    void (*on_element_open)(void* ctx, Str path, XmlParseAttributeList* attributes),
    void (*on_element_close)(void* ctx, Str path, Str content)
) {
    char* read = file_contents;
    char* file_end = file_contents + file_length;

    char path[256] = {0};
    char* path_write = path;

    XmlParseAttributeList attribute_list = {0};
    char* content_start = NULL;

    while (read < file_end) {
        char c = *read++;

        if (c != '<') {
            if (!content_start && !isspace(c)) {
                content_start = read - 1;
            }
            continue;
        }

        Str whole_tag = str_before_first_index('>', (Str){.start = read, .len = file_end - read});
        Str tag = str_trim(whole_tag);
        ASSERT(tag.len > 0);
        bool pop_path = false;

        if (tag.start[0] == '/') {
            ++tag.start;
            --tag.len;
            pop_path = true;

            Str content = content_start
                              ? (Str){.start = content_start, .len = read - 1 - content_start}
                              : (Str){0};

            Str str_path = (Str){.start = path, .len = path_write - path};
            on_element_close(user_ctx, str_path, content);

        } else if (tag.start[0] != '?') {
            pop_path = tag.start[tag.len - 1] == '/';
            if (pop_path) tag.len--;

            if (path_write != path && path_write < path + 256) {
                *path_write++ = '/';
            }

            attribute_list.count = 0;

            char* cur = tag.start;
            char* end = tag.start + tag.len;
            while (cur < end && !isspace(*cur)) ++cur;
            Str tag_name = {.start = tag.start, .len = cur - tag.start};

            PRINTF_BUF(path_write, path, 256, "%.*s", STR_PRINTF_ARGS(tag_name));

            while (cur < end) {
                while (cur < end && isspace(*cur)) ++cur;
                if (cur >= end) break;

                char* key_start = cur;
                while (cur < end && !isspace(*cur) && *cur != '=') ++cur;
                Str key = {.start = key_start, .len = cur - key_start};

                while (cur < end && isspace(*cur)) ++cur;
                ASSERT(cur < end && *cur == '=');
                ++cur;

                while (cur < end && isspace(*cur)) ++cur;
                ASSERT(cur < end);

                char quote = *cur;
                ASSERT(quote == '"' || quote == '\'');
                ++cur;

                char* val_start = cur;
                while (cur < end && *cur != quote) ++cur;
                ASSERT(cur < end);

                *DARRAY_PUSH(attribute_list) = (XmlParseAttribute){
                    .key = key,
                    .value = (Str){.start = val_start, .len = cur - val_start},
                };

                ++cur;
            }

            Str str_path = (Str){.start = path, .len = path_write - path};
            on_element_open(user_ctx, str_path, &attribute_list);
            if (pop_path) on_element_close(user_ctx, str_path, (Str){0});
        }

        read = whole_tag.start + whole_tag.len + 1;
        content_start = NULL;

        if (pop_path) {
            --path_write;
            while (*path_write != '/' && path_write > path) {
                *path_write = '\0';
                --path_write;
            }
            *path_write = '\0';
        }
    }
}