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
    for (u32 i = 0; i < 31 && i < str.count; ++i) {
        ret.value[i] = str.items[i];
    }
    return ret;
}

// --------------------------------------------------------------------------------------------------------------------

internal Str str_from_cstr(char* str) {
    return (Str){
        .items = str,
        .count = strlen(str),
    };
}

internal char* str_to_cstr(Arena* arena, Str str) {
    char* ret = arena_alloc(arena, str.count + 1);
    memcpy(ret, str.items, str.count);
    ret[str.count] = 0;
    return ret;
}

internal bool str_eq_cstr(char* cstr, Str a) {
    i32 cmp = strncmp(a.items, cstr, a.count);
    return cmp == 0 && cstr[a.count] == '\0';
}

internal Str str_before_first_index(char split, Str str) {
    char* end = memchr(str.items, split, str.count);
    if (end != NULL) {
        return (Str){
            .items = str.items,
            .count = end - str.items,
        };
    }
    return str;
}

internal Str str_after_first_index(char split, Str str) {
    char* end = memchr(str.items, split, str.count);
    if (end != NULL) {
        size_t fwd = end - str.items + 1;
        return (Str){
            .items = str.items + fwd,
            .count = str.count - fwd,
        };
    }
    return (Str){.items = NULL, .count = 0};
}

internal Str str_after_last_index(char split, Str str) {
    Str ret = (Str){
        .items = str.items + str.count - 1,
        .count = 1,
    };
    while (ret.items[0] != split) {
        ret.items--;
        ret.count++;
        if (ret.items == str.items) {
            return str;
        }
    }
    ret.items++;
    if (ret.count > 0) ret.count--;
    return ret;
}

internal void str_split_iter_next(StrSplitIter* it) {
    if (it->done) return;
    it->item_end_++;
    it->item.items = it->item_end_;
    if (it->item.items >= it->target_end_) {
        it->done = true;
        return;
    }
    while (it->item_end_ < it->target_end_ && *it->item_end_ != it->split) {
        it->item_end_++;
    }
    it->item.count = it->item_end_ - it->item.items;
}

internal StrSplitIter str_split_iter(char split, Str str) {
    StrSplitIter it = (StrSplitIter){
        .done = false,
        .split = split,
        .target = str,
        .item = {NULL, 0},
        .item_end_ = str.items - 1,
        .target_end_ = str.items + str.count,
    };
    str_split_iter_next(&it);
    return it;
}

internal void str_split_whitespace_iter_next(StrSplitWhitespaceIter* it) {
    if (it->done) return;
    while (it->item_end_ < it->target_end_ && isspace(*it->item_end_)) {
        it->item_end_++;
    }
    it->item.items = it->item_end_;
    if (it->item.items >= it->target_end_) {
        it->done = true;
        return;
    }
    while (it->item_end_ < it->target_end_ && !isspace(*it->item_end_)) {
        it->item_end_++;
    }
    it->item.count = it->item_end_ - it->item.items;
}
internal StrSplitWhitespaceIter str_split_whitespace_iter(Str str) {
    StrSplitWhitespaceIter it = (StrSplitWhitespaceIter){
        .done = false,
        .target = str,
        .item = {NULL, 0},
        .item_end_ = str.items,
        .target_end_ = str.items + str.count,
    };
    str_split_whitespace_iter_next(&it);
    return it;
}

internal bool str_eq(Str a, Str b) {
    if (a.count != b.count) return false;
    return strncmp(a.items, b.items, a.count) == 0;
}

internal Str str_trim(Str str) {
    if (str.count == 0) return str;
    str.count--;
    while (str.count > 0 && isspace(*str.items)) {
        str.items++;
        str.count--;
    }
    char* str_end = str.items + str.count;
    while (str.count > 0 && isspace(*str_end)) {
        str_end--;
        str.count--;
    }
    str.count++;
    return str;
}

internal Str str_copy(Arena* arena, Str src) {
    char* new_start = arena_alloc(arena, src.count);
    memcpy(new_start, src.items, src.count);
    return (Str){.items = new_start, .count = src.count};
}

internal i32 str_atoi(Str str) {
    i32 result = 0, sign = 1, i = 0;
    while (i < str.count && isspace(str.items[i])) {
        i++;
    }
    if (i < str.count) {
        if (str.items[i] == '-') {
            sign = -1;
            i++;
        } else if (str.items[i] == '+') {
            i++;
        }
    }
    for (; i < str.count && isdigit(str.items[i]); ++i) {
        result *= 10;
        result += str.items[i] - '0';
    }
    return sign * result;
}

internal bool str_starts_with_cstr(char* cstr, Str str) {
    i32 i = 0;
    for (; i < str.count && cstr[i]; ++i) {
        if (str.items[i] != cstr[i]) return false;
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