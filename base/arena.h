#pragma once

#define ARENA_MIN_ALIGNMENT 16

typedef void  (*ArenaDropFn)(void*);
typedef void* ArenaMark;

structdef(ArenaResourceNode) {
    ArenaResourceNode* next;
    void*              target;
    ArenaDropFn        drop;
};

structdef(Arena) {
    MemoryAllocator*   allocator;
    MemoryReservation  reservation;
    u8*                cur;
    ArenaResourceNode* resources_stack;
};

structdef(ArenaTemp) {
    Arena*    arena;
    ArenaMark mark;
};

structdef(ArenaArray) {
    Arena* arena;
    usize  elem_size;
    usize  count;
    void*  items;
};

internal Arena     arena_create(MemoryAllocator* allocator, usize block_size);
internal Arena*    arena_fork(Arena* self);
internal ArenaMark arena_mark(Arena* self);
internal void      arena_restore(Arena* self, ArenaMark saved);
internal void      arena_clear(Arena* self);
internal void      arena_destroy(Arena* self);

internal void* arena_alloc(Arena* self, usize size);
internal void* arena_alloc_nz(Arena* self, usize size);
internal void* arena_alloc_resource(Arena* self, usize size, ArenaDropFn drop);

internal ArenaArray arena_array_begin(Arena* self, usize elem_size);
internal void*      arena_array_push(ArenaArray* array);

internal void      scratch_thread_local_create(MemoryAllocator* allocator, usize block_size);
internal void      scratch_thread_local_destroy(void);
internal ArenaTemp scratch_acquire(Arena** conflicts, usize conflict_count);
internal void      scratch_release(ArenaTemp scratch);