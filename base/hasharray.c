#include "inc.h"

internal HashArray* hasharray_alloc(Arena* arena, size_t key_size, size_t value_size, size_t max_elems) {
    size_t capacity = NextPowerOf2(max_elems * 100 / HASHARRAY_LOAD_FACTOR_PERCENT);

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

internal u32 hasharray_djb2_hash(void* data, size_t len) {
    u32 hash = 5381;
    u8* str  = data;
    while (len > 0) {
        len--;
        hash = u32_wrapped_add(u32_wrapped_mul(33, hash), *str++);
    }
    return hash;
}

internal i64 hasharray_get_idx(HashArray* map, void* key) {
    u32 hash = hasharray_djb2_hash(key, map->key_size);
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
    Panic("Failed to find an empty hasharray slot (should never happen!)");
found:
    map->count++;
    map->hashes[i] = hash;
    memcpy((u8*)map->keys + map->key_size * i, key, map->key_size);

    void* value = (u8*)map->values + map->value_size * i;

    return memset(value, 0, map->value_size);
}

internal void* hasharray_insert(HashArray* map, void* key) {
    if (map->count >= map->max_elems) {
        Panic("Fixed-size hasharray is full and cannot be resized");
    }
    u32 hash = hasharray_djb2_hash(key, map->key_size);
    if (hash < 2) hash += 2;
    return hasharray_insert_with_hash(map, hash, key);
}

internal void* hasharray_get(HashArray* map, void* key) {
    i64 i = hasharray_get_idx(map, key);
    return i < 0 ? map->value_stub : (u8*)map->values + map->value_size * i;
}

internal void* hasharray_entry(HashArray* map, void* key) {
    i64 i = hasharray_get_idx(map, key);
    return i < 0 ? hasharray_insert(map, key) : (u8*)map->values + map->value_size * i;
}

internal bool hasharray_remove(HashArray* map, void* key) {
    i64 i = hasharray_get_idx(map, key);
    if (i < 0) return false;

    map->count--;
    map->hashes[i] = 1;  // tombstone
    return true;
}

internal void hasharray_clear(HashArray* map) {
    map->count = 0;
    memset(map->hashes, 0, map->capacity * sizeof(u32));
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