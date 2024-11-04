#pragma once

// Multi-producer single-consumer channel for communicating between threads
//
// Anyone can push onto the channel from any thread at any time, but the same
// thread should always be responsible for draining it.

typedef struct {
    size_t capacity;
    size_t item_size;
    atomic_uint_fast32_t count_commit[2];
    atomic_uint_fast32_t cur_buffer_reserve;
    void* buffer[2];
} Channel;

typedef struct {
    u32 count_;
    u32 idx_;
    size_t item_size_;
    void* buffer_;
    void* item;
    bool done;
} ChannelIter;

internal Channel channel_alloc(Arena* arena, size_t item_size, size_t capacity);
internal void channel_push(Channel* chan, void* item);

// for (ChannelIter it = channel_drain_iter(&chan); !it.done; channel_drain_iter_next(&it)) { ... }
internal ChannelIter channel_drain_iter(Channel* chan);
internal void channel_drain_iter_next(ChannelIter* it);

#if TEST
internal void test_channel(void);
#endif