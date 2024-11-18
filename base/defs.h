#pragma once
#if __STDC_VERSION__ < 201112L
#error "Codebase targets C11, which was not detected"
#endif

#ifndef DEBUG
#define DEBUG 0
#endif

#if DEBUG
#ifdef _MSC_VER
#define Trap() __debugbreak()
#else
#include <signal.h>
#define Trap() raise(SIGTRAP)
#endif
#else
#define Trap()
#endif

#if DEBUG
#define DebugAssert(x) Assert(x)
#define DEBUG_FN
#else
#define DebugAssert(x)
#define DEBUG_FN \
    {            \
    }
#endif

#define Max(a, b)          ((a) > (b) ? (a) : (b))
#define Min(a, b)          ((a) < (b) ? (a) : (b))
#define Clamp(x, min, max) (Max((min), Min((max), (x))))
#define Clamp01(x)         Clamp(x, 0, 1)
#define Abs(a)             ((a) >= 0 ? (a) : -(a))
#define Sign(a)            ((a) >= 0 ? 1 : -1)
#define NextPowerOf2(x)    ((x) <= 1 ? 1 : 1U << (32 - __builtin_clz((x) - 1)))

#define internal        static
#define global          static
#define local_persist   static
#define readonly_global static  // could use attributes to put this data in read-only pages, but that causes problems with dylib load on macos
#define thread_local    _Thread_local

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

#define Kb(n) (((u64)(n)) << 10)
#define Mb(n) (((u64)(n)) << 20)
#define Gb(n) (((u64)(n)) << 30)

#define Stringify(x)      #x
#define Concatenate(x, y) x##y

#define alignof(x) _Alignof(x)

#define ZeroStruct(struct_ptr) memset((struct_ptr), 0, sizeof(*(struct_ptr)))

#define Panic(...)                    \
    do {                              \
        fprintf(stderr, "Panic: ");   \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, "\n");        \
        Trap();                       \
        exit(EXIT_FAILURE);           \
    } while (0)

internal void panic_expr(char* msg) {
    Panic("%s", msg);
}

#define Log(...)                      \
    do {                              \
        fprintf(stderr, ">> ");       \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, "\n");        \
    } while (0)

#define Dbg(x) (                                                                                                                                                      \
    printf(">> %s:%i : %s = ", __FILE__, __LINE__, #x),                                                                                                               \
    printf((_Generic((x), u16: "%u", i16: "%i", u32: "%u", i32: "%i", u64: "%llu", i64: "%lli", size_t: "%lu", f32: "%f", f64: "%f", char*: "%s", bool: "%i")), (x)), \
    printf("\n"),                                                                                                                                                     \
    (x)                                                                                                                                                               \
)

#define DbgTyped(fmtstring, x, ...) (                   \
    printf(">> %s:%i : %s = ", __FILE__, __LINE__, #x), \
    printf(fmtstring, __VA_ARGS__),                     \
    printf("\n"),                                       \
    (x)                                                 \
)

#define Assert(x)                                                                  \
    do {                                                                           \
        if (!(x)) {                                                                \
            fprintf(stderr, "ASSERTION FAILED @ %s:%i : " #x, __FILE__, __LINE__); \
            Trap();                                                                \
            exit(EXIT_FAILURE);                                                    \
        }                                                                          \
    } while (0)

#define PrintfBuf(out_ptr, out_start_ptr, buf_len, ...)                            \
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

#define DefArrayTypes(type)       \
    typedef struct Slice_##type { \
        type* items;              \
        size_t count;             \
    } Slice_##type;               \
                                  \
    typedef struct Vec_##type {   \
        union {                   \
            struct {              \
                type* items;      \
                size_t count;     \
            };                    \
            Slice_##type slice;   \
        };                        \
        size_t capacity;          \
    } Vec_##type;

#define structdef(name)       \
    typedef struct name name; \
    struct name

#define uniondef(name)       \
    typedef union name name; \
    union name

DefArrayTypes(char);
DefArrayTypes(u8);
DefArrayTypes(i8);
DefArrayTypes(u16);
DefArrayTypes(i16);
DefArrayTypes(u32);
DefArrayTypes(i32);
DefArrayTypes(u64);
DefArrayTypes(i64);
DefArrayTypes(f32);
DefArrayTypes(f64);
DefArrayTypes(bool);
DefArrayTypes(size_t);

#define ArrayLen(arr) (sizeof(arr) / sizeof((arr)[0]))

#define VecAlloc(type, arena_ptr, capacity)                                    \
    (Vec_##type) {                                                             \
        {{arena_alloc((arena_ptr), (capacity) * sizeof(type)), 0}}, (capacity) \
    }

#define VecPush(vec) (                                                          \
    (vec).count < (vec).capacity                                                \
        ? &((vec).items[(vec).count++])                                         \
        : (panic_expr("Attempted to push onto a full vecay!"), &(vec).items[0]) \
)

#define VecPop(vec) (                                                       \
    (vec).count >= 1                                                        \
        ? &((vec).items[--(vec).count])                                     \
        : (panic_expr("Attempted to pop an empty vecay!"), &(vec).items[0]) \
)

#define StaticVec(type, capacity) \
    struct {                      \
        size_t count;             \
        type items[capacity];     \
    }

#define StaticVecPush(vec) (                                                         \
    (vec).count < ArrayLen((vec).items)                                              \
        ? &((vec).items[(vec).count++])                                              \
        : (panic_expr("Attempted to push onto a full static vec!"), &(vec).items[0]) \
)

#define StaticVecPop VecPop

#define SliceCopy(dest, src, start_idx, end_idx) \
    memcpy(&(dest)[start_idx], &(src)[start_idx], ((end_idx) - (start_idx)) * sizeof((dest)[0]))

#define SliceBinarySearch(i32_result_idx, slice, field_type, field, seeking) \
    do {                                                                     \
        (i32_result_idx) = -1;                                               \
        i32 left = 0;                                                        \
        i32 right = (slice).count - 1;                                       \
                                                                             \
        while (left <= right) {                                              \
            i32 mid = left + ((right - left) >> 1);                          \
            field_type found = (slice).items[mid] field;                     \
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

#define SliceSort(slice, comparator)                      \
    qsort(                                                \
        (slice).items,                                    \
        (slice).count,                                    \
        sizeof((slice).items[0]),                         \
        (i32(*)(const void*, const void*))(&(comparator)) \
    )

#define SllStackPush(stack_head, node) \
    do {                               \
        (node)->next = (stack_head);   \
        (stack_head) = (node);         \
    } while (0)

#define SllStackPop(stack_head)            \
    do {                                   \
        (stack_head) = (stack_head)->next; \
    } while (0)

#define SllStackPopInto(stack_head, node)  \
    do {                                   \
        (node) = (stack_head);             \
        (stack_head) = (stack_head)->next; \
        (node)->next = NULL;               \
    } while (0)

internal i32 sort_fn_f32_asc(f32* a, f32* b) { return *a > *b ? 1 : (*a < *b ? -1 : 0); }
internal i32 sort_fn_f32_desc(f32* a, f32* b) { return *a > *b ? -1 : (*a < *b ? 1 : 0); }
internal i32 sort_fn_i32_asc(i32* a, i32* b) { return *a - *b; }
internal i32 sort_fn_i32_desc(i32* a, i32* b) { return *b - *a; }

#if defined(__has_attribute)
#if __has_attribute(no_sanitize)
#define NO_SANITIZE_OVERFLOW __attribute__((no_sanitize("unsigned-integer-overflow", "signed-integer-overflow")))
#else
#define NO_SANITIZE_OVERFLOW
#endif
#else
#define NO_SANITIZE_OVERFLOW
#endif

NO_SANITIZE_OVERFLOW internal u32 u32_wrapped_add(u32 a, u32 b) { return a + b; }
NO_SANITIZE_OVERFLOW internal u32 u32_wrapped_mul(u32 a, u32 b) { return a * b; }
NO_SANITIZE_OVERFLOW internal u64 u64_wrapped_add(u64 a, u64 b) { return a + b; }
NO_SANITIZE_OVERFLOW internal u64 u64_wrapped_mul(u64 a, u64 b) { return a * b; }