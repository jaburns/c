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

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define CLAMP(x, min, max) (MAX((min), MIN((max), (x))))
#define CLAMP01(x) CLAMP(x, 0, 1)
#define ABS(a) ((a) >= 0 ? (a) : -(a))
#define SIGN(a) ((a) >= 0 ? 1 : -1)

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

#define int
#define float
#define double

#define KB(n) (((u64)(n)) << 10)
#define MB(n) (((u64)(n)) << 20)
#define GB(n) (((u64)(n)) << 30)

#define alignof(x) _Alignof(x)

#define ZERO_STRUCT(struct_ptr) (                   \
    memset((struct_ptr), 0, sizeof(*(struct_ptr))), \
    (struct_ptr)                                    \
)

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

#if DEBUG
#define DEBUG_ASSERT(x) ASSERT(x)
#define DEBUG_FN
#else
#define DEBUG_ASSERT(x)
#define DEBUG_FN \
    {            \
    }
#endif

#define ARRAY_LEN(arr) (sizeof(arr) / sizeof((arr)[0]))

#define ARRAY_BINARY_SEARCH(i32_result_idx, array, array_len, Type, field, seeking) \
    do {                                                                            \
        (i32_result_idx) = -1;                                                      \
        i32 left = 0;                                                               \
        i32 right = (array_len) - 1;                                                \
                                                                                    \
        while (left <= right) {                                                     \
            i32 mid = left + ((right - left) >> 1);                                 \
            Type found = (array)[mid] field;                                        \
                                                                                    \
            if (found == (seeking)) {                                               \
                (i32_result_idx) = mid;                                             \
                break;                                                              \
            } else if (found < (seeking)) {                                         \
                left = mid + 1;                                                     \
            } else {                                                                \
                right = mid - 1;                                                    \
            }                                                                       \
        }                                                                           \
    } while (0);

#define DARRAY(type, capacity) \
    struct {                   \
        size_t count;          \
        type items[capacity];  \
    }

#define DARRAY_PUSH(arr) (                                                       \
    (arr).count < ARRAY_LEN((arr).items)                                         \
        ? &((arr).items[(arr).count++])                                          \
        : (panic_expr("Attempted to push onto a full darray!"), &(arr).items[0]) \
)

#define DARRAY_POP(arr) (                                                    \
    (arr).count >= 1                                                         \
        ? &((arr).items[--(arr).count])                                      \
        : (panic_expr("Attempted to pop an empty darray!"), &(arr).items[0]) \
)

#define DARRAY_SORT(arr, comparator)                      \
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
