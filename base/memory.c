#include "inc.h"

internal MemoryReservation memory_reserve(void) {
    void* ptr = mmap(NULL, MEMORY_RESERVE_SIZE, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) PANIC("memory_reserve mmap failed");

    return (MemoryReservation){
        .base = ptr,
        .blocks_committed = 0,
    };
}

internal void memory_commit_size(MemoryReservation* reservation, size_t total_size) {
    size_t total_blocks_required = 1 + total_size / MEMORY_COMMIT_BLOCK_SIZE;

    if (total_blocks_required > reservation->blocks_committed) {
        size_t cur_size = MEMORY_COMMIT_BLOCK_SIZE * reservation->blocks_committed;
        size_t add_size = total_blocks_required - reservation->blocks_committed;
        reservation->blocks_committed = total_blocks_required;

        i32 result = mprotect(
            reservation->base + cur_size, add_size * MEMORY_COMMIT_BLOCK_SIZE, PROT_READ | PROT_WRITE
        );
        if (result == -1) PANIC("memory_commit_size mprotect failed");

    } else if (total_blocks_required < reservation->blocks_committed) {
        size_t remove_size = reservation->blocks_committed - total_blocks_required;
        reservation->blocks_committed = total_blocks_required;
        size_t new_size = MEMORY_COMMIT_BLOCK_SIZE * reservation->blocks_committed;

        i32 result = madvise(
            reservation->base + new_size, remove_size * MEMORY_COMMIT_BLOCK_SIZE, MADV_DONTNEED
        );
        if (result == -1) PANIC("memory_commit_size madvise failed");
    }
}

internal void memory_release(MemoryReservation* reservation) {
    i32 result = munmap(reservation->base, MEMORY_RESERVE_SIZE);
    if (result == -1) PANIC("memory_decommit munmap failed");
    ZERO_STRUCT(reservation);
}

internal void* memory_heap_alloc(size_t size) {
    return calloc(1, size);
}

internal void memory_heap_free(void* ptr) {
    free(ptr);
}

readonly_global MemoryAllocator GLOBAL_ALLOCATOR = (MemoryAllocator){
    .memory_reserve = memory_reserve,
    .memory_commit_size = memory_commit_size,
    .memory_release = memory_release,
    .memory_heap_alloc = memory_heap_alloc,
    .memory_heap_free = memory_heap_free,
};