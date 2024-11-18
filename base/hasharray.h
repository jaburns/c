#pragma once

#define HASHARRAY_LOAD_FACTOR_PERCENT 70

structdef(HashArray) {
    size_t capacity;
    size_t max_elems;
    size_t key_size;
    size_t value_size;

    size_t count;
    u32*   hashes;
    void*  keys;
    void*  values;
    void*  value_stub;
};

structdef(HashArrayIter) {
    size_t     idx_;
    HashArray* target;
    void*      key;
    void*      value;
    bool       done;
};

internal HashArray* hasharray_alloc(Arena* arena, size_t key_size, size_t value_size, size_t capacity);
internal void*      hasharray_insert(HashArray* map, void* key);
internal void*      hasharray_get(HashArray* map, void* key);
internal bool       hasharray_remove(HashArray* map, void* key);
internal void       hasharray_clear(HashArray* map);

internal HashArrayIter HashArrayIter_new(HashArray* map);
internal void          HashArrayIter_next(HashArrayIter* it);
