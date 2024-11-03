#include "inc.h"

// --------------------------------------------------------------------------------------------------------------------

internal Str32 str32_from_cstr(char* str) {
    Str32 ret = {0};
    strncpy(ret.value, str, 31);
    return ret;
}

internal bool str32_eq(Str32* a, Str32* b) {
    return memcmp(a->value, b->value, sizeof(a->value)) == 0;
}

internal bool str32_eq_cstr(char* cstr, Str32* a) {
    return strcmp((char*)a, cstr) == 0;
}

internal Str32 str32_from_str(Str str) {
    Str32 ret = {0};
    for (u32 i = 0; i < 31 && i < str.len; ++i) {
        ret.value[i] = str.start[i];
    }
    return ret;
}

// --------------------------------------------------------------------------------------------------------------------

internal Str str_from_cstr(char* str) {
    return (Str){
        .start = str,
        .len = strlen(str),
    };
}

internal char* str_to_cstr(Arena* arena, Str str) {
    char* ret = arena_alloc(arena, str.len + 1);
    memcpy(ret, str.start, str.len);
    ret[str.len] = 0;
    return ret;
}

internal bool str_eq_cstr(char* cstr, Str a) {
    i32 cmp = strncmp(a.start, cstr, a.len);
    return cmp == 0 && cstr[a.len] == '\0';
}

internal Str str_before_first_index(char split, Str str) {
    char* end = memchr(str.start, split, str.len);
    if (end != NULL) {
        return (Str){
            .start = str.start,
            .len = end - str.start,
        };
    }
    return str;
}

internal Str str_after_first_index(char split, Str str) {
    char* end = memchr(str.start, split, str.len);
    if (end != NULL) {
        size_t fwd = end - str.start + 1;
        return (Str){
            .start = str.start + fwd,
            .len = str.len - fwd,
        };
    }
    return (Str){.start = NULL, .len = 0};
}

internal Str str_after_last_index(char split, Str str) {
    Str ret = (Str){
        .start = str.start + str.len - 1,
        .len = 1,
    };
    while (ret.start[0] != split) {
        ret.start--;
        ret.len++;
        if (ret.start == str.start) {
            return str;
        }
    }
    ret.start++;
    if (ret.len > 0) ret.len--;
    return ret;
}

internal void str_split_iter_next(StrSplitIter* it) {
    if (it->done) return;
    it->item_end_++;
    it->item.start = it->item_end_;
    if (it->item.start >= it->target_end_) {
        it->done = true;
        return;
    }
    while (it->item_end_ < it->target_end_ && *it->item_end_ != it->split) {
        it->item_end_++;
    }
    it->item.len = it->item_end_ - it->item.start;
}

internal StrSplitIter str_split_iter(char split, Str str) {
    StrSplitIter it = (StrSplitIter){
        .done = false,
        .split = split,
        .target = str,
        .item = {NULL, 0},
        .item_end_ = str.start - 1,
        .target_end_ = str.start + str.len,
    };
    str_split_iter_next(&it);
    return it;
}

internal void str_split_whitespace_iter_next(StrSplitWhitespaceIter* it) {
    if (it->done) return;
    while (it->item_end_ < it->target_end_ && isspace(*it->item_end_)) {
        it->item_end_++;
    }
    it->item.start = it->item_end_;
    if (it->item.start >= it->target_end_) {
        it->done = true;
        return;
    }
    while (it->item_end_ < it->target_end_ && !isspace(*it->item_end_)) {
        it->item_end_++;
    }
    it->item.len = it->item_end_ - it->item.start;
}
internal StrSplitWhitespaceIter str_split_whitespace_iter(Str str) {
    StrSplitWhitespaceIter it = (StrSplitWhitespaceIter){
        .done = false,
        .target = str,
        .item = {NULL, 0},
        .item_end_ = str.start,
        .target_end_ = str.start + str.len,
    };
    str_split_whitespace_iter_next(&it);
    return it;
}

internal bool str_eq(Str a, Str b) {
    if (a.len != b.len) return false;
    return strncmp(a.start, b.start, a.len) == 0;
}

internal Str str_trim(Str str) {
    if (str.len == 0) return str;
    str.len--;
    while (str.len > 0 && isspace(*str.start)) {
        str.start++;
        str.len--;
    }
    char* str_end = str.start + str.len;
    while (str.len > 0 && isspace(*str_end)) {
        str_end--;
        str.len--;
    }
    str.len++;
    return str;
}

internal Str str_copy(Arena* arena, Str src) {
    char* new_start = arena_alloc(arena, src.len);
    memcpy(new_start, src.start, src.len);
    return (Str){.start = new_start, .len = src.len};
}

internal i32 str_atoi(Str str) {
    i32 result = 0, sign = 1, i = 0;
    while (i < str.len && isspace(str.start[i])) {
        i++;
    }
    if (i < str.len) {
        if (str.start[i] == '-') {
            sign = -1;
            i++;
        } else if (str.start[i] == '+') {
            i++;
        }
    }
    for (; i < str.len && isdigit(str.start[i]); ++i) {
        result *= 10;
        result += str.start[i] - '0';
    }
    return sign * result;
}

internal bool str_starts_with_cstr(char* cstr, Str str) {
    i32 i = 0;
    for (; i < str.len && cstr[i]; ++i) {
        if (str.start[i] != cstr[i]) return false;
    }
    return !cstr[i];
}

// --------------------------------------------------------------------------------------------------------------------

internal char* read_file(Arena* arena, char* filename, size_t* out_length) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        PANIC("Failed to open file: %s", filename);
    }

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* content = arena_alloc_not_zeroed(arena, file_size + 1);
    if (fread(content, 1, file_size, file) != file_size && ferror(file)) {
        PANIC("Failed to read file: %s", filename);
    }

    content[file_size] = 0;
    fclose(file);

    if (out_length != NULL) {
        *out_length = file_size;
    }
    return content;
}

// --------------------------------------------------------------------------------------------------------------------