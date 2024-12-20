#include "inc.h"

internal HashArray* hasharray_alloc_(Arena* arena, usize key_size, usize value_size, usize capacity, usize max_elems) {
    u32*       hashes     = arena_alloc(arena, capacity * sizeof(u32));
    void*      keys       = arena_alloc(arena, capacity * key_size);
    void*      values     = arena_alloc(arena, capacity * value_size);
    void*      value_stub = arena_alloc(arena, value_size);
    HashArray* map        = arena_alloc(arena, sizeof(HashArray));

    *map = (HashArray){
        .capacity   = capacity,
        .max_elems  = max_elems,
        .key_size   = key_size,
        .value_size = value_size,

        .count      = 0,
        .hashes     = hashes,
        .keys       = keys,
        .values     = values,
        .value_stub = value_stub,
    };

    return map;
}

internal HashArray* hasharray_alloc_with_cap(Arena* arena, usize key_size, usize value_size, usize capacity) {
    capacity        = NextPowerOf2(capacity);
    usize max_elems = capacity * HASHARRAY_LOAD_FACTOR_PERCENT / 100;
    return hasharray_alloc_(arena, key_size, value_size, capacity, max_elems);
}

internal HashArray* hasharray_alloc_with_elems(Arena* arena, usize key_size, usize value_size, usize max_elems) {
    usize capacity = NextPowerOf2(max_elems * 100 / HASHARRAY_LOAD_FACTOR_PERCENT);
    return hasharray_alloc_(arena, key_size, value_size, capacity, max_elems);
}

NO_SANITIZE_OVERFLOW u32 hasharray_murmur3_32_hash(void* data, usize len) {
    u8* bytes   = (u8*)data;
    i32 nblocks = len / 4;
    u32 h1      = 0x87C263D1;  // seed

    for (i32 i = 0; i < nblocks; i++) {
        u32 k1;
        memcpy(&k1, bytes + i * 4, 4);
        k1 *= 0xCC9E2D51;
        k1  = (k1 << 15) | (k1 >> 17);
        k1 *= 0x1B873593;
        h1 ^= k1;
        h1  = (h1 << 13) | (h1 >> 19);
        h1  = h1 * 5 + 0xE6546B64;
    }

    u8* tail = bytes + nblocks * 4;
    u32 k1   = 0;

    switch (len & 3) {
        case 3:
            k1 ^= tail[2] << 16;
        case 2:
            k1 ^= tail[1] << 8;
        case 1:
            k1 ^= tail[0];
            k1 *= 0xCC9E2D51;
            k1  = (k1 << 15) | (k1 >> 17);
            k1 *= 0x1B873593;
            h1 ^= k1;
    };

    h1 ^= len;
    h1 ^= h1 >> 16;
    h1 *= 0x85EBCA6B;
    h1 ^= h1 >> 13;
    h1 *= 0xC2B2AE35;
    h1 ^= h1 >> 16;

    return h1;
}

internal u32 hasharray_find_idx(HashArray* map, void* key) {
    u32 hash = hasharray_murmur3_32_hash(key, map->key_size);
    if (hash < 2) hash += 2;
    u32 start_idx = hash & (map->capacity - 1);
    u32 i;

#define X()                                                                                                   \
    {                                                                                                         \
        u32 stored_hash = map->hashes[i];                                                                     \
        if (stored_hash == 0) return UINT32_MAX;                                                              \
        if (stored_hash > 1 && memcmp(key, (u8*)map->keys + map->key_size * i, map->key_size) == 0) return i; \
    }

    for (i = start_idx; i < map->capacity; ++i) X();
    for (i = 0; i < start_idx; ++i) X();

#undef X

    return UINT32_MAX;
}

internal void* hasharray_get(HashArray* map, void* key) {
    u32 i = hasharray_find_idx(map, key);
    return i == UINT32_MAX ? map->value_stub : (u8*)map->values + map->value_size * i;
}

internal bool hasharray_has(HashArray* map, void* key) {
    return hasharray_find_idx(map, key) != UINT32_MAX;
}

internal void* hasharray_insert(HashArray* map, void* key) {
    if (map->count >= map->max_elems) {
        Panic("Fixed-size hasharray is full and cannot be resized");
    }

    u32 hash = hasharray_murmur3_32_hash(key, map->key_size);
    if (hash < 2) hash += 2;

    u32 start_idx = hash & (map->capacity - 1);
    u32 i;

    for (i = start_idx; i < map->capacity; ++i) {
        if (map->hashes[i] < 2) goto found;
    }
    for (i = 0; i < start_idx; ++i) {
        if (map->hashes[i] < 2) goto found;
    }

    unreachable();

found:
    map->count++;
    map->hashes[i] = hash;
    memcpy((u8*)map->keys + map->key_size * i, key, map->key_size);

    void* value = (u8*)map->values + map->value_size * i;

    return memset(value, 0, map->value_size);
}

internal void* hasharray_entry(HashArray* map, void* key) {
    u32 hash = hasharray_murmur3_32_hash(key, map->key_size);
    if (hash < 2) hash += 2;

    u32 start_idx     = hash & (map->capacity - 1);
    u32 tombstone_idx = UINT32_MAX;
    u32 i;

#define X()                                                                            \
    {                                                                                  \
        u32 stored_hash = map->hashes[i];                                              \
        if (stored_hash > 1) {                                                         \
            if (memcmp(key, (u8*)map->keys + map->key_size * i, map->key_size) == 0) { \
                return (u8*)map->values + map->value_size * i;                         \
            }                                                                          \
        } else if (stored_hash == 1) {                                                 \
            if (tombstone_idx == UINT32_MAX) tombstone_idx = i;                        \
        } else {                                                                       \
            goto not_found;                                                            \
        }                                                                              \
    }

    for (i = start_idx; i < map->capacity; ++i) X();
    for (i = 0; i < start_idx; ++i) X();

#undef X

    unreachable();

not_found:
    if (map->count >= map->max_elems) {
        Panic("Fixed-size hasharray is full and cannot be resized");
    }
    if (tombstone_idx < UINT32_MAX) i = tombstone_idx;

    map->count++;
    map->hashes[i] = hash;
    memcpy((u8*)map->keys + map->key_size * i, key, map->key_size);

    void* value = (u8*)map->values + map->value_size * i;

    return memset(value, 0, map->value_size);
}

internal bool hasharray_remove(HashArray* map, void* key) {
    u32 i = hasharray_find_idx(map, key);
    if (i == UINT32_MAX) return false;

    map->count--;
    map->hashes[i] = 1;  // tombstone
    return true;
}

internal void hasharray_clear(HashArray* map) {
    map->count = 0;
    ZeroArray(map->hashes, map->capacity);
}

internal HashArrayIter HashArrayIter_new(HashArray* map) {
    HashArrayIter it = {
        .idx_   = -1,
        .target = map,
        .key    = NULL,
        .value  = NULL,
        .done   = false,
    };
    HashArrayIter_next(&it);
    return it;
}

internal void HashArrayIter_next(HashArrayIter* it) {
    it->idx_ = u32_wrapped_add(it->idx_, 1);
    for (;;) {
        if (it->idx_ >= it->target->capacity) {
            it->done = true;
            return;
        }
        if (it->target->hashes[it->idx_] > 1) {
            it->key   = (u8*)it->target->keys + it->target->key_size * it->idx_;
            it->value = (u8*)it->target->values + it->target->value_size * it->idx_;
            return;
        }
        ++it->idx_;
    }
}