#include "inc.h"

internal void xml_parse(
    Arena* arena,
    char* file_contents,
    size_t file_length,
    void* user_ctx,
    void (*on_element_open)(void* ctx, Str path, Slice_XmlParseAttribute attributes),
    void (*on_element_close)(void* ctx, Str path, Str content)
) {
    char* read = file_contents;
    char* file_end = file_contents + file_length;

    char path[256] = {0};
    char* path_write = path;

    Array_XmlParseAttribute attributes = ARRAY_ALLOC(XmlParseAttribute, arena, 256);

    char* content_start = NULL;

    while (read < file_end) {
        char c = *read++;

        if (c != '<') {
            if (!content_start && !isspace(c)) {
                content_start = read - 1;
            }
            continue;
        }

        Str whole_tag = str_before_first_index('>', (Str){.items = read, .count = file_end - read});
        Str tag = str_trim(whole_tag);
        ASSERT(tag.count > 0);
        bool pop_path = false;

        if (tag.items[0] == '/') {
            ++tag.items;
            --tag.count;
            pop_path = true;

            Str content = content_start
                              ? (Str){.items = content_start, .count = read - 1 - content_start}
                              : (Str){0};

            Str str_path = (Str){.items = path, .count = path_write - path};
            on_element_close(user_ctx, str_path, content);

        } else if (tag.items[0] != '?') {
            pop_path = tag.items[tag.count - 1] == '/';
            if (pop_path) tag.count--;

            if (path_write != path && path_write < path + 256) {
                *path_write++ = '/';
            }

            attributes.count = 0;

            char* cur = tag.items;
            char* end = tag.items + tag.count;
            while (cur < end && !isspace(*cur)) ++cur;
            Str tag_name = {.items = tag.items, .count = cur - tag.items};

            PRINTF_BUF(path_write, path, 256, "%.*s", STR_PRINTF_ARGS(tag_name));

            while (cur < end) {
                while (cur < end && isspace(*cur)) ++cur;
                if (cur >= end) break;

                char* key_start = cur;
                while (cur < end && !isspace(*cur) && *cur != '=') ++cur;
                Str key = {.items = key_start, .count = cur - key_start};

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

                *ARRAY_PUSH(attributes) = (XmlParseAttribute){
                    .key = key,
                    .value = (Str){.items = val_start, .count = cur - val_start},
                };

                ++cur;
            }

            Str str_path = (Str){.items = path, .count = path_write - path};
            on_element_open(user_ctx, str_path, attributes.slice);
            if (pop_path) on_element_close(user_ctx, str_path, (Str){0});
        }

        read = whole_tag.items + whole_tag.count + 1;
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