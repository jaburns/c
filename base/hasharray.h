#pragma once

#define HASHARRAY_LOAD_FACTOR_PERCENT 70

structdef(HashArray) {
    usize capacity;
    usize max_elems;
    usize key_size;
    usize value_size;

    usize count;
    u32*  hashes;
    void* keys;
    void* values;
    void* value_stub;
};

structdef(HashArrayIter) {
    usize      idx_;
    HashArray* target;
    void*      key;
    void*      value;
    bool       done;
};

internal HashArray* hasharray_alloc_with_cap(Arena* arena, usize key_size, usize value_size, usize capacity);
internal HashArray* hasharray_alloc_with_elems(Arena* arena, usize key_size, usize value_size, usize max_elems);

internal void* hasharray_insert(HashArray* map, void* key);
internal void* hasharray_get(HashArray* map, void* key);
internal void* hasharray_entry(HashArray* map, void* key);
internal bool  hasharray_remove(HashArray* map, void* key);
internal void  hasharray_clear(HashArray* map);

internal HashArrayIter HashArrayIter_new(HashArray* map);
internal void          HashArrayIter_next(HashArrayIter* it);
