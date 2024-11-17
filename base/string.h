#pragma once

structdef(Str32) {
    char value[31];
    char zero;
};

typedef Slice_char Str;

structdef(StrSplitIter) {
    bool done;
    char split;
    Str target;
    Str item;
    char* target_end_;
    char* item_end_;
};

structdef(StrSplitWhitespaceIter) {
    bool done;
    Str target;
    Str item;
    char* target_end_;
    char* item_end_;
};

internal Str32 str32_from_cstr(char* str);
internal bool str32_eq(Str32* a, Str32* b);
internal bool str32_eq_cstr(char* cstr, Str32* a);
internal Str32 str32_from_str(Str str);

// usage: printf(" %.*s ", STR_PRINTF_ARGS(some_str));
#define StrPrintfArgs(str) (i32)(str).count, (str).items

#define DbgStr(v) DBG_TYPED("\"%.*s\"", (v), STR_PRINTF_ARGS(v))

#define CstrLen(cstr_lit) (sizeof(cstr_lit) - 1)

#define Str(cstr_lit) ((Str){(cstr_lit), CSTR_LEN(cstr_lit)})

internal Str str_from_cstr(char* str);
internal char* str_to_cstr(Arena* arena, Str str);
internal bool str_eq_cstr(char* cstr, Str a);

internal Str str_before_first_index(char split, Str str);
internal Str str_after_first_index(char split, Str str);
internal Str str_after_last_index(char split, Str str);

internal void str_split_iter_next(StrSplitIter* it);
internal StrSplitIter str_split_iter(char split, Str str);
internal void str_split_whitespace_iter_next(StrSplitWhitespaceIter* it);
internal StrSplitWhitespaceIter str_split_whitespace_iter(Str str);

internal Str str_trim(Str str);
internal bool str_eq(Str a, Str b);
internal Str str_copy(Arena* arena, Str src);
internal i32 str_atoi(Str str);
internal bool str_starts_with_cstr(char* cstr, Str str);
internal Str str_substr_from(Str str, size_t idx);

internal char* u64_print_with_commas(Arena* arena, u64 num);

internal char* read_file(Arena* arena, char* filename, size_t* out_length);
internal Str str_read_file(Arena* arena, char* filename);