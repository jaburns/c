#include "inc.h"

// --------------------------------------------------------------------------------------------------------------------

internal Channel channel_alloc(Arena* arena, size_t item_size, size_t capacity) {
    size_t buffer_size = capacity * item_size;
    u8* buffers = arena_alloc(arena, 2 * buffer_size);

    return (Channel){
        .capacity = capacity,
        .item_size = item_size,
        .buffer = {buffers, buffers + buffer_size},
    };
}

internal void channel_push(Channel* chan, void* item) {
    u32 cur_buf_reserve = atomic_fetch_add_explicit(&chan->cur_buffer_reserve, 1, memory_order_relaxed);
    u32 buf = cur_buf_reserve & 0x80000000 ? 1 : 0;
    u32 reserved_idx = cur_buf_reserve & 0x7fffffff;

    if (reserved_idx >= chan->capacity) {
        Panic("Channel is full");
    }

    void* target = (u8*)chan->buffer[buf] + chan->item_size * reserved_idx;
    memcpy(target, item, chan->item_size);

    atomic_fetch_add_explicit(&chan->count_commit[buf], 1, memory_order_release);
}

internal ChannelIter ChannelIter_new(Channel* chan) {
    u32 prev_peek = atomic_load_explicit(&chan->cur_buffer_reserve, memory_order_acquire);
    u32 new_value = prev_peek & 0x80000000 ? 0 : 0x80000000;
    u32 prev_buf_reserve = atomic_exchange_explicit(&chan->cur_buffer_reserve, new_value, memory_order_acq_rel);

    u32 prev_buf = prev_peek & 0x80000000 ? 1 : 0;
    u32 prev_reserved_idx = prev_buf_reserve & 0x7fffffff;

    for (;;) {
        u32 check = atomic_load_explicit(&chan->count_commit[prev_buf], memory_order_acquire);
        if (check == prev_reserved_idx) break;
    }

    *(u32*)&chan->count_commit[prev_buf] = 0;

    ChannelIter it = {
        .count_ = prev_reserved_idx,
        .idx_ = -1,
        .item_size_ = chan->item_size,
        .buffer_ = chan->buffer[prev_buf],
    };

    ChannelIter_next(&it);

    return it;
};

internal void ChannelIter_next(ChannelIter* it) {
    ++it->idx_;
    if (it->idx_ >= it->count_) {
        it->done = true;
    } else {
        it->item = (u8*)it->buffer_ + it->item_size_ * it->idx_;
    }
}

// --------------------------------------------------------------------------------------------------------------------

#if TEST

#define THREAD_COUNT 32
#define NUM_ITEMS    32768

global atomic_bool test_channel_start;
global atomic_bool test_channel_done;

internal void* test_channel_push_thread(void* arg) {
    while (!test_channel_start);

    Channel* chan = (Channel*)arg;
    for (u64 i = 1; i <= NUM_ITEMS; ++i) {
        channel_push(chan, &i);
    }
    return NULL;
}

internal void* test_channel_drain_thread(void* arg) {
    while (!test_channel_start);

    Channel* chan = (Channel*)arg;
    u64 sum = 0;
    bool should_finish = false;

    for (;;) {
        foreach (ChannelIter, it, chan) {
            u64* item = (u64*)it.item;
            if (*item == 0) Panic("");
            sum += *item;
        }

        if (should_finish) break;
        if (test_channel_done) should_finish = true;
    }

    return (void*)(u64)sum;
}

internal void test_channel(void) {
    Arena arena = arena_create(&GLOBAL_ALLOCATOR);

    for (i32 i = 0; i < 10; ++i) {
        test_channel_start = false;
        test_channel_done = false;

        arena_clear(&arena);

        Channel chan = channel_alloc(&arena, sizeof(u64), THREAD_COUNT * NUM_ITEMS);

        pthread_t threads[THREAD_COUNT];
        pthread_t drain_thread;

        pthread_create(&drain_thread, NULL, test_channel_drain_thread, (void*)&chan);
        for (u64 i = 0; i < THREAD_COUNT; ++i) {
            pthread_create(&threads[i], NULL, test_channel_push_thread, (void*)&chan);
        }

        test_channel_start = true;

        for (u64 i = 0; i < THREAD_COUNT; ++i) {
            pthread_join(threads[i], NULL);
        }

        test_channel_done = true;

        void* result;
        pthread_join(drain_thread, &result);
        u64 sum = (u64)result;

        u64 expected_sum = (u64)THREAD_COUNT * (NUM_ITEMS * (NUM_ITEMS + 1)) / 2;

        if (sum != expected_sum) {
            Panic("Expected %llu but got %llu\n", expected_sum, sum);
        }
    }

    arena_destroy(&arena);
}

#undef THREAD_COUNT
#undef NUM_ITEMS

#endif