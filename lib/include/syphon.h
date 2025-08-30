#ifndef SYPHON_H
#define SYPHON_H

#include <stddef.h>   // size_t

#include "../src/defines.h"

/* ----------- DARRAY -----------*/
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
b8 da_push(DArray *arr, const void *elem);

/* Remove the last element and copy it into `out` (if non‑NULL).
 * Returns true on success, false if the array is empty. */
b8 da_pop(DArray *arr, void *out);

b8 da_get(const DArray *arr, size_t index, void *out);

void *da_at(DArray *arr, size_t index);

/* Const version for read‑only access */
const void *da_at_const(const DArray *arr, size_t index);

/* Release all memory owned by the array and set its fields to zero. */
void da_free(DArray *arr);

/* ----------- END DARRAY -----------------*/

// structs
typedef struct syp_core_info_t {
    u16 index;
    f64 used;
    f64 free;
} syp_core_info_t;

typedef struct syp_proc_info_t {
    u32 pid;
    const char *command;
    const char *name;

//     struct  proc *e_paddr;          /* address of proc */
// 		struct  session *e_sess;        /* session pointer */
// 		struct  _pcred e_pcred;         /* process credentials */
// 		struct  _ucred e_ucred;         /* current credentials */
// 		struct   vmspace e_vm;          /* address space */
// 		pid_t   e_ppid;                 /* parent process id */
// 		pid_t   e_pgid;                 /* process group id */
// 		short   e_jobc;                 /* job control counter */
// 		dev_t   e_tdev;                 /* controlling tty dev */
// 		pid_t   e_tpgid;                /* tty process group id */
// 		struct  session *e_tsess;       /* tty session pointer */
// #define WMESGLEN        7
// 		char    e_wmesg[WMESGLEN + 1];    /* wchan message */
// 		segsz_t e_xsize;                /* text size */
// 		short   e_xrssize;              /* text rss */
// 		short   e_xccount;              /* text references */
// 		short   e_xswrss;
// 		int32_t e_flag;
// #define EPROC_CTTY      0x01    /* controlling tty vnode active */
// #define EPROC_SLEADER   0x02    /* session leader */
// #define COMAPT_MAXLOGNAME       12
// 		char    e_login[COMAPT_MAXLOGNAME];     /* short setlogin() name */
} syp_proc_info_t;


typedef struct syphon_info_t {
    //CPU
    u16 core_count;
    DArray *cores_info; // array of syp_core_info_t
    u16 proc_count;
    DArray *procs_info; // array of syp_proc_info_t

    // MEMORY
    u64 mem_total; // bytes
    u64 mem_used;  // bytes
    u64 mem_free;  // bytes
    
    // SWAP
    u64 swap_total;  // bytes
    u64 swap_used;   // bytes
    u64 swap_free;   // bytes
    
    // FILESYSTEM
    u64 fs_total;  // bytes
    u64 fs_free;   // bytes
    u64 fs_avail;  // bytes

    // NETWORK
    u64 net_total_tx; // bytes transmitted since boot on all interfaces
    u64 net_total_rx; // bytes recieved since boot on all interfaces

    // DEVICES
    
} syphon_info_t;

SAPI syphon_info_t syphon_info_create();
SAPI void syphon_info_get(syphon_info_t *info);
SAPI void syphon_info_print(syphon_info_t *info);
SAPI void syphon_info_cleanup(syphon_info_t *info);


#endif