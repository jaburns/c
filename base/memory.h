#pragma once

#define MEMORY_RESERVE_SIZE Gb(32)

#ifndef MEMORY_COMMIT_BLOCK_SIZE
#define MEMORY_COMMIT_BLOCK_SIZE Mb(1)
#endif

structdef(MemoryReservation) {
    u8*    base;
    size_t blocks_committed;
};

structdef(MemoryAllocator) {
    MemoryReservation (*memory_reserve)(void);
    void              (*memory_commit_size)(MemoryReservation* reservation, size_t total_size);
    void              (*memory_release)(MemoryReservation* reservation);
    void*             (*memory_heap_alloc)(size_t size);
    void              (*memory_heap_free)(void* ptr);
};

readonly_global MemoryAllocator GLOBAL_ALLOCATOR;