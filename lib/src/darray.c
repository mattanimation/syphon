#include <stdlib.h>
#include <string.h>

#include "darray.h"

static bool da_grow(DArray *arr, size_t min_capacity) {
    size_t new_cap = arr->capacity ? arr->capacity * 2 : 1;
    while (new_cap < min_capacity) new_cap *= 2;

    void *tmp = realloc(arr->data, new_cap * arr->elem_size);
    if (!tmp) return false;               // allocation failed
    arr->data = tmp;
    arr->capacity = new_cap;
    return true;
}

DArray *da_alloc(size_t elem_size, size_t init_cap) {
    if (elem_size == 0) return NULL;
    DArray *arr = malloc(sizeof *arr);
    if (!arr) return NULL;

    arr->elem_size = elem_size;
    arr->length    = 0;
    arr->capacity  = 0;
    arr->data      = NULL;

    if (init_cap && !da_grow(arr, init_cap)) {
        free(arr);
        return NULL;
    }
    return arr;
}

bool da_push(DArray *arr, const void *elem) {
    if (arr->length == arr->capacity) {
        if (!da_grow(arr, arr->length + 1)) return false;
    }
    // Copy the element into the next slot
    memcpy((char *)arr->data + arr->length * arr->elem_size,
           elem,
           arr->elem_size);
    ++arr->length;
    return true;
}

bool da_pop(DArray *arr, void *out) {
    if (arr->length == 0) return false;   // under‑flow
    --arr->length;
    if (out) {
        memcpy(out,
               (char *)arr->data + arr->length * arr->elem_size,
               arr->elem_size);
    }
    return true;
}

void da_free(DArray *arr) {
    if (!arr) return;
    free(arr->data);
    arr->data = NULL;
    arr->capacity = arr->length = 0;
    arr->elem_size = 0;
}