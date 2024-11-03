#include "inc.h"

internal HashArray* hasharray_alloc(Arena* arena, size_t key_size, size_t value_size, size_t max_elems) {
    size_t capacity = NEXT_POWER_OF_2(max_elems * 100 / HASHARRAY_LOAD_FACTOR_PERCENT);

    u32* hashes = arena_alloc(arena, capacity * sizeof(u32));
    void* keys = arena_alloc(arena, capacity * key_size);
    void* values = arena_alloc(arena, capacity * value_size);
    void* value_stub = arena_alloc(arena, value_size);
    HashArray* map = arena_alloc(arena, sizeof(HashArray));

    *map = (HashArray){
        .capacity = capacity,
        .max_elems = max_elems,
        .key_size = key_size,
        .value_size = value_size,

        .count = 0,
        .hashes = hashes,
        .keys = keys,
        .values = values,
        .value_stub = value_stub,
    };

    return map;
}

internal u32 hasharray_murmur3_32_hash(void* data, size_t len) {
    u8* bytes = (u8*)data;
    i32 nblocks = len / 4;
    u32 h1 = 0x87c263d1;  // seed
    u32* blocks = (u32*)(bytes + nblocks * 4);

    for (i32 i = -nblocks; i; i++) {
        u32 k1 = blocks[i];
        k1 *= 0xcc9e2d51;
        k1 = (k1 << 15) | (k1 >> 17);
        k1 *= 0x1b873593;
        h1 ^= k1;
        h1 = (h1 << 13) | (h1 >> 19);
        h1 = h1 * 5 + 0xe6546b64;
    }

    u8* tail = bytes + nblocks * 4;
    u32 k1 = 0;

    switch (len & 3) {
        case 3:
            k1 ^= tail[2] << 16;
        case 2:
            k1 ^= tail[1] << 8;
        case 1:
            k1 ^= tail[0];
            k1 *= 0xcc9e2d51;
            k1 = (k1 << 15) | (k1 >> 17);
            k1 *= 0x1b873593;
            h1 ^= k1;
    };

    h1 ^= len;
    h1 ^= h1 >> 16;
    h1 *= 0x85ebca6b;
    h1 ^= h1 >> 13;
    h1 *= 0xc2b2ae35;
    h1 ^= h1 >> 16;

    return h1;
}

internal u32 hasharray_get_idx(HashArray* map, void* key) {
    u32 hash = hasharray_murmur3_32_hash(key, map->key_size);
    if (hash < 2) hash += 2;
    u32 start_idx = hash & (map->capacity - 1);
    u32 i;
    for (i = start_idx; i < map->capacity; ++i) {
        if (map->hashes[i] == 0) return -1;
        if (map->hashes[i] > 1 && memcmp(key, (u8*)map->keys + map->key_size * i, map->key_size) == 0) return i;
    }
    for (i = 0; i < start_idx; ++i) {
        if (map->hashes[i] == 0) return -1;
        if (map->hashes[i] > 1 && memcmp(key, (u8*)map->keys + map->key_size * i, map->key_size) == 0) return i;
    }
    return -1;
}

internal void* hasharray_insert_with_hash(HashArray* map, u32 hash, void* key) {
    u32 start_idx = hash & (map->capacity - 1);
    u32 i;
    for (i = start_idx; i < map->capacity; ++i) {
        if (map->hashes[i] < 2) goto found;
    }
    for (i = 0; i < start_idx; ++i) {
        if (map->hashes[i] < 2) goto found;
    }
    PANIC("Failed to find an empty hasharray slot (should never happen!)");
found:
    map->count++;
    map->hashes[i] = hash;
    memcpy((u8*)map->keys + map->key_size * i, key, map->key_size);

    void* value = (u8*)map->values + map->value_size * i;
    memset(value, 0, map->value_size);

    return value;
}

internal void* hasharray_insert(HashArray* map, void* key) {
    if (map->count >= map->max_elems) {
        PANIC("Fixed-size hasharray is full and cannot be resized");
    }
    u32 hash = hasharray_murmur3_32_hash(key, map->key_size);
    if (hash < 2) hash += 2;
    return hasharray_insert_with_hash(map, hash, key);
}

internal void* hasharray_get(HashArray* map, void* key) {
    u32 i = hasharray_get_idx(map, key);
    return i < 0 ? map->value_stub : (u8*)map->values + map->value_size * i;
}

internal bool hasharray_remove(HashArray* map, void* key) {
    u32 i = hasharray_get_idx(map, key);
    if (i < 0) return false;

    map->count--;
    map->hashes[i] = 1;  // tombstone
    return true;
}

internal void hasharray_clear(HashArray* map) {
    map->count = 0;
    memset(map->hashes, 0, map->capacity * sizeof(u32));
}

internal HashArrayIter hasharray_iter(HashArray* map) {
    HashArrayIter it = {
        .idx_ = -1,
        .target = map,
        .key = NULL,
        .value = NULL,
        .done = false,
    };
    hasharray_iter_next(&it);
    return it;
}

internal void hasharray_iter_next(HashArrayIter* it) {
    ++it->idx_;
    for (;;) {
        if (it->idx_ >= it->target->capacity) {
            it->done = true;
            return;
        }
        if (it->target->hashes[it->idx_] > 1) {
            it->key = (u8*)it->target->keys + it->target->key_size * it->idx_;
            it->value = (u8*)it->target->values + it->target->value_size * it->idx_;
            return;
        }
        ++it->idx_;
    }
}