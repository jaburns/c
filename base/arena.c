#include "inc.h"

global thread_local Arena g_arena_scratch[2];

internal Arena arena_create(MemoryAllocator* allocator) {
    MemoryReservation reservation = allocator->memory_reserve();
    return (Arena){
        .allocator       = allocator,
        .reservation     = reservation,
        .cur             = reservation.base,
        .resources_stack = NULL,
    };
}

internal Arena* arena_fork(Arena* self) {
    Arena* child = arena_alloc_resource(self, sizeof(Arena), (ArenaDropFn)arena_destroy);
    *child       = arena_create(self->allocator);
    return child;
}

internal ArenaMark arena_mark(Arena* self) {
    return self->cur;
}

internal void arena_restore(Arena* self, ArenaMark saved) {
    while (self->resources_stack && self->resources_stack->target >= saved) {
        self->resources_stack->drop(self->resources_stack->target);
        SllStackPop(self->resources_stack);
    }
    self->cur = saved;
    self->allocator->memory_commit_size(&self->reservation, self->cur - self->reservation.base);
}

internal void arena_clear(Arena* self) {
    while (self->resources_stack) {
        self->resources_stack->drop(self->resources_stack->target);
        SllStackPop(self->resources_stack);
    }
    self->cur = self->reservation.base;
    self->allocator->memory_commit_size(&self->reservation, 0);
}

internal void arena_destroy(Arena* self) {
    arena_clear(self);
    self->allocator->memory_release(&self->reservation);
    ZeroStruct(self);
}

internal void arena_align(Arena* arena) {
    arena->cur = (void*)(((uintptr_t)arena->cur + (alignof(max_align_t) - 1)) & ~(alignof(max_align_t) - 1));
}

internal ArenaArray arena_array_begin(Arena* arena, size_t elem_size) {
    arena_align(arena);
    return (ArenaArray){
        .arena     = arena,
        .elem_size = elem_size,
        .items     = arena->cur,
    };
}

internal void* arena_array_push(ArenaArray* array) {
    u8* ret = array->arena->cur;
    array->arena->allocator->memory_commit_size(&array->arena->reservation, (array->arena->cur - array->arena->reservation.base) + array->elem_size);
    array->arena->cur += array->elem_size;
    ++array->count;
    return memset(ret, 0, array->elem_size);
}

internal void* arena_alloc(Arena* self, size_t size) {
    void* ret = arena_alloc_not_zeroed(self, size);
    return memset(ret, 0, size);
}

internal void* arena_alloc_not_zeroed(Arena* self, size_t size) {
    arena_align(self);
    u8* ret = self->cur;
    self->allocator->memory_commit_size(&self->reservation, (self->cur - self->reservation.base) + size);
    self->cur += size;
    return ret;
}

internal void* arena_alloc_resource(Arena* self, size_t size, ArenaDropFn drop) {
    void*              result = arena_alloc(self, size);
    ArenaResourceNode* node   = arena_alloc(self, sizeof(ArenaResourceNode));

    node->target = result;
    node->drop   = drop;
    SllStackPush(self->resources_stack, node);

    return result;
}

internal void scratch_thread_local_create(MemoryAllocator* allocator) {
    g_arena_scratch[0] = arena_create(allocator);
    g_arena_scratch[1] = arena_create(allocator);
}

internal void scratch_thread_local_destroy(void) {
    arena_destroy(&g_arena_scratch[0]);
    arena_destroy(&g_arena_scratch[1]);
}

internal ArenaTemp scratch_acquire(Arena** conflicts, size_t conflict_count) {
    bool matched[2] = {0};
    for (u32 i = 0; i < conflict_count; ++i) {
        if (&g_arena_scratch[0] == conflicts[i]) {
            matched[0] = true;
            if (matched[1]) goto err;
        } else if (&g_arena_scratch[1] == conflicts[i]) {
            matched[1] = true;
            if (matched[0]) goto err;
        }
    }

    Arena*    arena = &g_arena_scratch[matched[0] ? 1 : 0];
    ArenaMark mark  = arena_mark(arena);

    return (ArenaTemp){
        .arena = arena,
        .mark  = mark,
    };
err:
    Panic("Both scratch arenas passed as conflicts to scratch_acquire");
}

internal void scratch_release(ArenaTemp scratch) {
    arena_restore(scratch.arena, scratch.mark);
}