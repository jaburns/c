#pragma once
#if __STDC_VERSION__ < 201112L
#error "Codebase targets C11, which was not detected"
#endif

#ifndef DEBUG
#define DEBUG 0
#endif

#if DEBUG
#ifdef _MSC_VER
#define TRAP() __debugbreak()
#else
#include <signal.h>
#define TRAP() raise(SIGTRAP)
#endif
#else
#define TRAP()
#endif

#if DEBUG
#define DEBUG_ASSERT(x) ASSERT(x)
#define DEBUG_FN
#else
#define DEBUG_ASSERT(x)
#define DEBUG_FN \
    {            \
    }
#endif

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define CLAMP(x, min, max) (MAX((min), MIN((max), (x))))
#define CLAMP01(x) CLAMP(x, 0, 1)
#define ABS(a) ((a) >= 0 ? (a) : -(a))
#define SIGN(a) ((a) >= 0 ? 1 : -1)
#define LENGTH(arr) (sizeof(arr) / sizeof((arr)[0]))

#define NEXT_POWER_OF_2(x) ((x) <= 1 ? 1 : 1U << (32 - __builtin_clz((x) - 1)))

#define internal static
#define global static
#define local_persist static
#define readonly_global static  // could use attributes to put this data in read-only pages, but that causes problems with dylib load on macos

#define thread_local _Thread_local

typedef uint8_t u8;
typedef int8_t i8;
typedef uint8_t u16;
typedef int8_t i16;
typedef uint32_t u32;
typedef int32_t i32;
typedef uint64_t u64;
typedef int64_t i64;
typedef float f32;
typedef double f64;

#define KB(n) (((u64)(n)) << 10)
#define MB(n) (((u64)(n)) << 20)
#define GB(n) (((u64)(n)) << 30)

#define alignof(x) _Alignof(x)

#define ZERO_STRUCT(struct_ptr) memset((struct_ptr), 0, sizeof(*(struct_ptr)))

#define PANIC(...)                    \
    do {                              \
        fprintf(stderr, "PANIC: ");   \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, "\n");        \
        TRAP();                       \
        exit(EXIT_FAILURE);           \
    } while (0)

internal void panic_expr(char* msg) {
    PANIC("%s", msg);
}

#define LOG(...)                      \
    do {                              \
        fprintf(stderr, ">> ");       \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, "\n");        \
    } while (0)

#define DBG(x) (                                                                                                                                                    \
    printf(">> %s:%i : %s = ", __FILE__, __LINE__, #x),                                                                                                             \
    printf((_Generic((x), u16: "%u", i16: "%i", u32: "%u", i32: "%i", u64: "%lu", i64: "%li", size_t: "%lu", f32: "%f", f64: "%f", char*: "%s", bool: "%i")), (x)), \
    printf("\n"),                                                                                                                                                   \
    (x)                                                                                                                                                             \
)

#define DBG_TYPED(fmtstring, x, ...) (                  \
    printf(">> %s:%i : %s = ", __FILE__, __LINE__, #x), \
    printf(fmtstring, __VA_ARGS__),                     \
    printf("\n"),                                       \
    (x)                                                 \
)

#define ASSERT(x)                                                                  \
    do {                                                                           \
        if (!(x)) {                                                                \
            fprintf(stderr, "ASSERTION FAILED @ %s:%i : " #x, __FILE__, __LINE__); \
            TRAP();                                                                \
            exit(EXIT_FAILURE);                                                    \
        }                                                                          \
    } while (0)

#define PRINTF_BUF(out_ptr, out_start_ptr, buf_len, ...)                           \
    do {                                                                           \
        i32 remaining = (i32)(buf_len) - (i32)((out_ptr) - (out_start_ptr));       \
        if (remaining > 0) {                                                       \
            i32 written = snprintf((out_ptr), remaining, __VA_ARGS__);             \
            (out_ptr) += written > 0 && written < remaining ? written : remaining; \
        }                                                                          \
    } while (0)

// Use underlying type syntax if available, otherwise ignore size in debug builds in favor
// of getting errors for non-exhaustive switches, but use specified underlying type directly
// in release builds.
#if __STDC_VERSION__ >= 202311L
#define enumdef(name, type) \
    typedef enum name name; \
    enum name : type
#elif DEBUG
#define enumdef(name, type) \
    typedef enum name name; \
    enum name
#else
#define enumdef(name, type) \
    typedef type name;      \
    enum name
#endif

#define DEF_ARRAY_TYPES(type)     \
    typedef struct Slice_##type { \
        type* items;              \
        size_t count;             \
    } Slice_##type;               \
                                  \
    typedef struct Array_##type { \
        union {                   \
            struct {              \
                type* items;      \
                size_t count;     \
            };                    \
            Slice_##type slice;   \
        };                        \
        size_t capacity;          \
    } Array_##type;

#define structdef(name)       \
    typedef struct name name; \
    DEF_ARRAY_TYPES(name);    \
    struct name

#define uniondef(name)       \
    typedef union name name; \
    DEF_ARRAY_TYPES(name);   \
    union name

DEF_ARRAY_TYPES(char);
DEF_ARRAY_TYPES(u8);
DEF_ARRAY_TYPES(i8);
DEF_ARRAY_TYPES(u16);
DEF_ARRAY_TYPES(i16);
DEF_ARRAY_TYPES(u32);
DEF_ARRAY_TYPES(i32);
DEF_ARRAY_TYPES(u64);
DEF_ARRAY_TYPES(i64);
DEF_ARRAY_TYPES(f32);
DEF_ARRAY_TYPES(f64);
DEF_ARRAY_TYPES(bool);
DEF_ARRAY_TYPES(size_t);

#define ARRAY_ALLOC(type, arena_ptr, capacity)                                 \
    (Array_##type) {                                                           \
        {{arena_alloc((arena_ptr), (capacity) * sizeof(type)), 0}}, (capacity) \
    }

#define ARRAY_PUSH(arr) (                                                       \
    (arr).count < (arr).capacity                                                \
        ? &((arr).items[(arr).count++])                                         \
        : (panic_expr("Attempted to push onto a full array!"), &(arr).items[0]) \
)

#define ARRAY_POP(arr) (                                                    \
    (arr).count >= 1                                                        \
        ? &((arr).items[--(arr).count])                                     \
        : (panic_expr("Attempted to pop an empty array!"), &(arr).items[0]) \
)

#define SARRAY(type, capacity) \
    struct {                   \
        size_t count;          \
        type items[capacity];  \
    }

#define SARRAY_PUSH(arr) (                                                       \
    (arr).count < LENGTH((arr).items)                                            \
        ? &((arr).items[(arr).count++])                                          \
        : (panic_expr("Attempted to push onto a full darray!"), &(arr).items[0]) \
)

#define SARRAY_POP ARRAY_POP

#define ARRAY_BINARY_SEARCH(i32_result_idx, arr, field_type, field, seeking) \
    do {                                                                     \
        (i32_result_idx) = -1;                                               \
        i32 left = 0;                                                        \
        i32 right = (arr).count - 1;                                         \
                                                                             \
        while (left <= right) {                                              \
            i32 mid = left + ((right - left) >> 1);                          \
            field_type found = (arr).items[mid] field;                       \
                                                                             \
            if (found == (seeking)) {                                        \
                (i32_result_idx) = mid;                                      \
                break;                                                       \
            } else if (found < (seeking)) {                                  \
                left = mid + 1;                                              \
            } else {                                                         \
                right = mid - 1;                                             \
            }                                                                \
        }                                                                    \
    } while (0)

#define ARRAY_SORT(arr, comparator)                       \
    qsort(                                                \
        (arr).items,                                      \
        (arr).count,                                      \
        sizeof((arr).items[0]),                           \
        (i32(*)(const void*, const void*))(&(comparator)) \
    )

#define SLL_STACK_PUSH(stack_head, node) \
    do {                                 \
        (node)->next = (stack_head);     \
        (stack_head) = (node);           \
    } while (0)

#define SLL_STACK_POP(stack_head)          \
    do {                                   \
        (stack_head) = (stack_head)->next; \
    } while (0)

#define SLL_STACK_POP_INTO(stack_head, node) \
    do {                                     \
        (node) = (stack_head);               \
        (stack_head) = (stack_head)->next;   \
        (node)->next = NULL;                 \
    } while (0)

internal i32 sort_fn_f32_asc(f32* a, f32* b) { return *a > *b ? 1 : (*a < *b ? -1 : 0); }
internal i32 sort_fn_f32_desc(f32* a, f32* b) { return *a > *b ? -1 : (*a < *b ? 1 : 0); }
internal i32 sort_fn_i32_asc(i32* a, i32* b) { return *a - *b; }
internal i32 sort_fn_i32_desc(i32* a, i32* b) { return *b - *a; }