#ifndef DARRAY_H
#define DARRAY_H

#include <stddef.h>   // size_t
#include <stdbool.h>  // bool

typedef struct {
    void   *data;          // pointer to the raw buffer
    size_t  elem_size;     // size of each element (bytes)
    size_t  length;        // number of elements currently stored
    size_t  capacity;      // allocated slots
} DArray;

/* Allocate a new dynamic array.
 *  elem_size – size of a single element (e.g. sizeof(int))
 *  init_cap  – initial capacity (can be 0; will be grown on first push)
 * Returns NULL on allocation failure. */
DArray *da_alloc(size_t elem_size, size_t init_cap);

/* Append a copy of the element pointed to by `elem`.
 * Returns true on success, false on allocation failure. */
bool da_push(DArray *arr, const void *elem);

/* Remove the last element and copy it into `out` (if non‑NULL).
 * Returns true on success, false if the array is empty. */
bool da_pop(DArray *arr, void *out);

/* Release all memory owned by the array and set its fields to zero. */
void da_free(DArray *arr);

#endif /* DRRAY_H */