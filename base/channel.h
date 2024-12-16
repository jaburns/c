#pragma once

// Multi-producer single-consumer channel for communicating between threads
//
// Anyone can push onto the channel from any thread at any time, but the same
// thread should always be responsible for draining it.

structdef(Channel) {
    usize                capacity;
    usize                item_size;
    atomic_uint_fast32_t count_commit[2];
    atomic_uint_fast32_t cur_buffer_reserve;
    void*                buffer[2];
};

structdef(ChannelIter) {
    u32   count_;
    u32   idx_;
    usize item_size_;
    void* buffer_;
    void* item;
    bool  done;
};

internal Channel channel_alloc(Arena* arena, usize item_size, usize capacity);
internal void    channel_push(Channel* chan, void* item);

internal ChannelIter ChannelIter_new(Channel* chan);
internal void        ChannelIter_next(ChannelIter* it);

#if TEST
internal void test_channel(void);
#endif