#pragma once

structdef(Str32) {
    u8   count;
    char items[31];
};
DefArrayTypes(Str32);

typedef Slice_char Str;

structdef(StrSplitIter) {
    bool  done;
    char  split;
    Str   target;
    Str   item;
    char* target_end_;
    char* item_end_;
};

structdef(StrSplitWhitespaceIter) {
    bool  done;
    Str   target;
    Str   item;
    char* target_end_;
    char* item_end_;
};

internal void str32_push(Str32* str, char c);
internal void str32_push_repeat(Str32* str, char c, u32 num);
// internal Str32 str32_from_cstr(char* str);
// internal bool  str32_eq(Str32* a, Str32* b);
// internal bool  str32_eq_cstr(char* cstr, Str32* a);
// internal Str32 str32_from_str(Str str);

// usage: printf(" %.*s ", STR_PRINTF_ARGS(some_str));
#define StrPrintfArgs(str) (i32)(str).count, (str).items

#define Dbg_Str(v) DbgTyped("\"%.*s\"", (v), StrPrintfArgs(v))

#define CstrLen(cstr_lit) (sizeof(cstr_lit) - 1)

#define Str(cstr_lit) ((Str){(cstr_lit), CstrLen(cstr_lit)})

internal Str   str_from_cstr(char* str);
internal char* str_to_cstr(Arena* arena, Str str);
internal bool  str_eq_cstr(char* cstr, Str a);

internal Str str_before_first_index(char split, Str str);
internal Str str_after_first_index(char split, Str str);
internal Str str_after_last_index(char split, Str str);

internal StrSplitIter StrSplitIter_new(char split, Str str);
internal void         StrSplitIter_next(StrSplitIter* it);

internal StrSplitWhitespaceIter StrSplitWhitespaceIter_new(Str str);
internal void                   StrSplitWhitespaceIter_next(StrSplitWhitespaceIter* it);

internal Str  str_trim(Str str);
internal bool str_eq(Str a, Str b);
internal Str  str_copy(Arena* arena, Str src);
internal u64  str_parse_u64(Str str, i32 base);
internal i64  str_parse_i64(Str str, i32 base);
internal i32  str_parse_u32(Str str, i32 base);
internal i32  str_parse_i32(Str str, i32 base);
internal bool str_starts_with_cstr(char* cstr, Str str);
internal Str  str_substr_to(Str str, usize idx);
internal Str  str_substr_from(Str str, usize idx);
internal Str  str_substr(Str str, usize idx, usize len);

internal char* u64_print_with_commas(Arena* arena, u64 num);

internal char* read_file(Arena* arena, char* filename, size_t* out_length);
internal Str   str_read_file(Arena* arena, char* filename);