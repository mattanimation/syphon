
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// for disk info
#include <sys/statvfs.h>

#define SEXPORT
#define SRELEASE 0

#include <syphon.h>


//#include "darray.h"

/**
 * // Create a dynamic array for ints, start with capacity 4
    DynArray *vec = da_alloc(sizeof(int), 4);
    if (!vec) { perror("alloc"); return 1; }

    // Push some numbers 
    for (int i = 0; i < 10; ++i) {
        if (!da_push(vec, &i)) {
            fprintf(stderr, "push failed at %d\n", i);
            da_free(vec);
            return 1;
        }
    }

    // Pop and print them in reverse order 
    int value;
    while (da_pop(vec, &value)) {
        printf("%d ", value);
    }
    putchar('\n');

    da_free(vec);
 */

/** --------- DARRAY --------------- */
static b8 da_grow(DArray *arr, size_t min_capacity) {
    size_t new_cap = arr->capacity ? arr->capacity * 2 : 1;
    while (new_cap < min_capacity) new_cap *= 2;

    void *tmp = realloc(arr->data, new_cap * arr->elem_size);
    if (!tmp) return FALSE;               // allocation failed
    arr->data = tmp;
    arr->capacity = new_cap;
    return TRUE;
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

b8 da_push(DArray *arr, const void *elem) {
    if (arr->length == arr->capacity) {
        if (!da_grow(arr, arr->length + 1)) return FALSE;
    }
    // Copy the element into the next slot
    memcpy((char *)arr->data + arr->length * arr->elem_size,
           elem,
           arr->elem_size);
    ++arr->length;
    return TRUE;
}

b8 da_pop(DArray *arr, void *out) {
    if (arr->length == 0) return FALSE;   // under‑flow
    --arr->length;
    if (out) {
        memcpy(out,
               (char *)arr->data + arr->length * arr->elem_size,
               arr->elem_size);
    }
    return TRUE;
}

b8 da_get(const DArray *arr, size_t index, void *out)
{
    if (!arr || index >= arr->length)          // bounds check
        return FALSE;

    if (out) {
        /* Compute the address of the indexed slot and copy it out */
        const char *src = (const char *)arr->data + index * arr->elem_size;
        memcpy(out, src, arr->elem_size);
    }
    return TRUE;
}

/* Helper that does the bounds check and computes the address */
static inline void *_element_ptr(DArray *arr, size_t idx)
{
    return (char *)arr->data + idx * arr->elem_size;
}

/* mutable get value at index */
void *da_at(DArray *arr, size_t index)
{
    if (!arr || index >= arr->length)   /* out of range → safety guard */
        return NULL;
    return _element_ptr(arr, index);
}

/* read only get value at index */
const void *da_at_const(const DArray *arr, size_t index)
{
    if (!arr || index >= arr->length)
        return NULL;
    return (const char *)arr->data + index * arr->elem_size;
}

void da_free(DArray *arr) {
    if (!arr) return;
    free(arr->data);
    arr->data = NULL;
    arr->capacity = arr->length = 0;
    arr->elem_size = 0;
}

/** ---------- END DARRAY ------------- */

const u64 gib = 1024 * 1024 * 1024;
const u64 mib = 1024 * 1024;
const u64 kib = 1024;

f64 bytes_to_kibs(const u64 value) {
    if(value == 0) {return value;} // avoid zero division
    return value / kib;
}

f64 bytes_to_mibs(const u64 value) {
    if(value == 0) {return value;} // avoid zero division
    return value / mib;
}

f64 bytes_to_gibs(const u64 value) {
    if(value == 0) {return value;} // avoid zero division
    return value / gib;
}

/**
 * Creates a default info struct to store
 * all the system informaiton on
 */
syphon_info_t syphon_info_create() {

    DArray *core_info_darray = da_alloc(sizeof(syp_core_info_t), 32);
    DArray *proc_info_darray = da_alloc(sizeof(syp_proc_info_t), 32);
   

    return (syphon_info_t){
        .core_count=0,
        .cores_info=core_info_darray,
        .procs_info=proc_info_darray,
        .fs_avail=0,
        .fs_free=0,
        .fs_total=0,
        .mem_free=0,
        .mem_total=0,
        .mem_used=0,
        .swap_total=0,
        .swap_used=0,
        .swap_free=0,
        .net_total_tx=0,
        .net_total_rx=0
    };
}

SAPI void syphon_info_cleanup(syphon_info_t *info){
    // need to cleanup and free any darray memory;
    da_free(info->cores_info);
    da_free(info->procs_info);
}

// COMMON POSIX CODE THAT WORKS CROSS PLATFORMS
void get_fs_stats(const char *path, syphon_info_t *info) {
    struct statvfs sv;
    if (statvfs(path, &sv) != 0) {
        perror("statvfs");
        return;
    }
    u64 blockSize = sv.f_frsize;   /* fundamental block size */
    // u64 total = sv.f_blocks * blockSize;
    // u64 free  = sv.f_bfree  * blockSize;
    // u64 avail = sv.f_bavail * blockSize; /* for unprivileged users */

    info->fs_total = sv.f_blocks * blockSize;
    info->fs_free = sv.f_bfree * blockSize;
    info->fs_avail = sv.f_bavail * blockSize;

    // printf("Filesystem \"%s\":\n", path);
    // printf("  Total: %llu bytes\n", total);
    // printf("  Free : %llu bytes (available to non‑root: %llu)\n",
    //        free, avail);
}

void syphon_info_print(syphon_info_t *info) {
    printf("---- SYPHON INFO START ----\n");

    printf("CPU (info):\n");
    printf("Detected %u cores\n", info->core_count);
    printf("size of cores info: %zu\n", info->cores_info->length);
    for(u32 i = 0; i < info->cores_info->length; ++i){
        syp_core_info_t ci;
        if(da_get(info->cores_info, i, &ci)){
            printf("Core %2u: %.1f%% busy (idle %.1f%%)\n", ci.index, ci.used, ci.free);
        }
        
    }

    printf("PROCESS (info):\n");
    printf("Detected %u processes\n", info->proc_count);
    for(u32 i = 0; i < info->procs_info->length; ++i){
        // limit to 10 for now since so many
        if(i < 10){
            syp_proc_info_t pi;
            if(da_get(info->procs_info, i, &pi)){
                printf("PID: %2u | name: %s command: %s \n", pi.pid, pi.name, pi.command);
            }
        }
    }

    printf("Memory (bytes):\n");
    printf("  Total: %llu\n", info->mem_total);
    printf("  Used : %llu\n", info->mem_used);
    printf("  Free : %llu\n", info->mem_free);

    printf("Memory (kbytes):\n");
    printf("  Total: %.2f\n", bytes_to_kibs(info->mem_total));
    printf("  Used : %.2f\n", bytes_to_kibs(info->mem_used));
    printf("  Free : %.2f\n", bytes_to_kibs(info->mem_free));

    printf("Memory (mbytes):\n");
    printf("  Total: %.2f\n", bytes_to_mibs(info->mem_total));
    printf("  Used : %.2f\n", bytes_to_mibs(info->mem_used));
    printf("  Free : %.2f\n", bytes_to_mibs(info->mem_free));

    printf("Memory (gbytes):\n");
    printf("  Total: %.2f\n", bytes_to_gibs(info->mem_total));
    printf("  Used : %.2f\n", bytes_to_gibs(info->mem_used));
    printf("  Free : %.2f\n", bytes_to_gibs(info->mem_free));
    
    printf("Swap (bytes):\n");
    printf("  Total: %llu\n", info->swap_total);
    printf("  Used : %llu\n", info->swap_used);
    printf("  Free : %llu\n", info->swap_free);
    
    printf("Filesystem \"%s\":\n", "/");
    printf("  Total: %llu bytes\n", info->fs_total);
    printf("  Free : %llu bytes (available to non‑root: %llu)\n", info->fs_free, info->fs_avail);

    printf("Filesystem \"%s\":\n", "/");
    printf("  Total: %.2f kb\n", bytes_to_kibs(info->fs_total));
    printf("  Free : %.2f kb (available to non‑root: %.2f)\n", bytes_to_kibs(info->fs_free), bytes_to_kibs(info->fs_avail));

    printf("Filesystem \"%s\":\n", "/");
    printf("  Total: %.2f mb\n", bytes_to_mibs(info->fs_total));
    printf("  Free : %.2f mb (available to non‑root: %.2f)\n", bytes_to_mibs(info->fs_free), bytes_to_mibs(info->fs_avail));

    printf("Filesystem \"%s\":\n", "/");
    printf("  Total: %.2f gb\n", bytes_to_gibs(info->fs_total));
    printf("  Free : %.2f gb (available to non‑root: %.2f)\n", bytes_to_gibs(info->fs_free), bytes_to_gibs(info->fs_avail));
    
    printf("Network traffic (total bytes since boot):\n");
    printf("   Total TX: %llu \n", info->net_total_tx);
    printf("   Total RX: %llu \n", info->net_total_rx);

    printf("---- SYPHON INFO END ----\n");
}

#if PLATFORM_APPLE

// cpu and memory
#include <mach/mach.h>
#include <mach/processor_info.h>
#include <mach/mach_host.h>
// for procs
#include <sys/sysctl.h>
#include <sys/types.h>
#include <sys/user.h>   // for struct kinfo_proc
#include <errno.h>

// network
#include <ifaddrs.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>


void get_mem_stats(syphon_info_t *info) {

    mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
    vm_statistics64_data_t vmstat;
    kern_return_t kr_mem = host_statistics64(mach_host_self(),
                                         HOST_VM_INFO64,
                                         (host_info64_t)&vmstat,
                                         &count);
    if (kr_mem != KERN_SUCCESS) {
        fprintf(stderr, "host_statistics64 failed: %d\n", kr_mem);
        return;
    }

    /* Page size in bytes */
    vm_size_t pageSize;
    host_page_size(mach_host_self(), &pageSize);

    int64_t mem_used = (vmstat.active_count +
                        vmstat.inactive_count +
                        vmstat.wire_count) * pageSize;
    int64_t mem_free = vmstat.free_count * pageSize;

    info->mem_used = mem_used;
    info->mem_free = mem_free;
    info->mem_total = mem_used + mem_free;
    // NOTE: not sure about these...
    info->swap_used = vmstat.swapins;
    info->swap_free = vmstat.swapouts;
    info->swap_total = vmstat.swapins + vmstat.swapouts;

}

void get_cpu_stats(syphon_info_t *info) {
    u32   cpuCount;
    processor_info_array_t   cpuInfo;
    mach_msg_type_number_t   cpuInfoCnt;
    kern_return_t    kr_cpu;

    /* Get a snapshot of the current CPU load */
    kr_cpu = host_processor_info(mach_host_self(),
                             PROCESSOR_CPU_LOAD_INFO,
                             &cpuCount,
                             &cpuInfo,
                             &cpuInfoCnt);
    if (kr_cpu != KERN_SUCCESS) {
        fprintf(stderr, "host_processor_info failed: %d\n", kr_cpu);
        return;
    }

    //printf("Detected %u cores\n", cpuCount);
    info->core_count = cpuCount;

    for (u16 i = 0; i < cpuCount; ++i) {
        
        i32 *cpuTicks = &cpuInfo[i * CPU_STATE_MAX];
        u64 total = 0;
        
        for (int j = 0; j < CPU_STATE_MAX; ++j) {
            total += cpuTicks[j];
        }

        /* Calculate % busy = 100 * (user+system+nice) / total */
        f64 busy = 100.0 *
            (cpuTicks[CPU_STATE_USER] +
             cpuTicks[CPU_STATE_SYSTEM] +
             cpuTicks[CPU_STATE_NICE]) / (f64)total;

        // printf("Core %2u: %.1f%% busy (idle %.1f%%)\n",
        //        i, busy, 100.0 - busy);
        
        
        if(info->cores_info->length >= cpuCount){
            // set the values
            syp_core_info_t *ci = (syp_core_info_t *)da_at(info->cores_info, i);
            if(ci){
                ci->free = 100.0 - busy;
                ci->index = i;
                ci->used = busy;
            }
        } else {
            syp_core_info_t ci = {
                .free=100.0 - busy,
                .index=i,
                .used=busy
            };
            //push the values
            if (!da_push(info->cores_info, &ci)) {
                fprintf(stderr, "push failed at %d\n", i);
                da_free(info->cores_info);
                return;
            }
        }
        
    }

    /* Release the buffer allocated by host_processor_info */
    vm_deallocate(mach_task_self(), (vm_address_t)cpuInfo, cpuInfoCnt);
}

void get_proc_stats(syphon_info_t *info) {
    int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0 };
    size_t len = 0;
    int ret = sysctl(mib, 4, NULL, &len, NULL, 0);
    if (ret == -1) {
        perror("sysctl size query");
        return;
    }

    struct kinfo_proc *procList = malloc(len);
    if (!procList) {
        perror("malloc");
        return;
    }

    ret = sysctl(mib, 4, procList, &len, NULL, 0);
    if (ret == -1) {
        perror("sysctl data query");
        free(procList);
        return;
    }

    size_t count = len / sizeof(struct kinfo_proc);
    //printf("Found %zu processes:\n", count);
    info->proc_count = count;

    for (size_t i = 0; i < count; ++i) {
        pid_t pid = procList[i].kp_proc.p_pid;
        const char *name = procList[i].kp_proc.p_comm;

        //printf("%5d  %s\n", pid, name);

        if(info->cores_info->length >= count){
            // set the values
            syp_proc_info_t *pi = (syp_proc_info_t *)da_at(info->procs_info, i);
            if(pi){
                pi->name = name;
                pi->pid = pid;
                pi->command = "what?";
            }
        } else {
            syp_proc_info_t pi = {
                .pid=pid,
                .name=name,
                .command="what?"
            };
            //push the values
            if (!da_push(info->procs_info, &pi)) {
                fprintf(stderr, "push failed at %zu\n", i);
                da_free(info->procs_info);
                return;
            }
        }
    }

    free(procList);
}

void get_network_stats(syphon_info_t * info) {
    struct ifaddrs *ifap, *ifa;
    if (getifaddrs(&ifap) != 0) {
        perror("getifaddrs");
        return;
    }

    u64 ttlTx = 0;
    u64 ttlRx = 0;
    for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) continue;
        if (ifa->ifa_data == NULL) continue;          /* skip non‑data interfaces */

        struct if_data *ifd = (struct if_data *)ifa->ifa_data;
        // printf("%-10s  TX: %llu  RX: %llu\n",
        //        ifa->ifa_name,
        //        (u64)ifd->ifi_obytes,
        //        (u64)ifd->ifi_ibytes);
        ttlTx += (u64)ifd->ifi_obytes;
        ttlRx += (u64)ifd->ifi_ibytes;
    }
    freeifaddrs(ifap);
    info->net_total_tx = ttlTx;
    info->net_total_rx = ttlRx;
}

void get_device_stats(syphon_info_t *info) {

}


void syphon_info_get(syphon_info_t *info) {
    

    printf("GET INFO: %s\n", PLATFORM_APPLE ? "mac" : "?");

    // ---------- CPU ----------
    get_cpu_stats(info);

    // ---------- PROCS ---------
    get_proc_stats(info);

    // ---------- NETWORK ----------
    get_network_stats(info);

    // --------- MEMORY INFO ----------
    get_mem_stats(info);

    // --------- DISK INFO ----------
    get_fs_stats("/", info);

    // --------- DEVICES ----------
    get_device_stats(info);

    //get_fs_stats("/home", &info);

}

#endif

#if PLATFORM_LINUX

#include <string.h>
#include <unistd.h>


typedef struct {
    u64 user, nice, system, idle, iowait, irq, softirq, steal;
} CpuStat;

/* Parse a single line like: "cpu0  12345 678 9012 34567 ..." */
static int parse_cpu_line(const char *line, CpuStat *out) {
    char label[16];
    int n = sscanf(line,
        "%15s %llu %llu %llu %llu %llu %llu %llu %llu",
        label,
        &out->user, &out->nice, &out->system,
        &out->idle, &out->iowait,
        &out->irq, &out->softirq, &out->steal);
    return (n >= 5) ? 0 : -1;   // we need at least user‑nice‑system‑idle
}

/* Compute % busy from two snapshots */
static double calc_busy(const CpuStat *old, const CpuStat *new) {
    u64 oldIdle = old->idle + old->iowait;
    u64 newIdle = new->idle + new->iowait;

    u64 oldTotal = old->user + old->nice + old->system +
                                 old->idle + old->iowait + old->irq +
                                 old->softirq + old->steal;
    u64 newTotal = new->user + new->nice + new->system +
                                 new->idle + new->iowait + new->irq +
                                 new->softirq + new->steal;

    u64 totalDiff = newTotal - oldTotal;
    u64 idleDiff  = newIdle  - oldIdle;

    if (totalDiff == 0) return 0.0;
    return 100.0 * (totalDiff - idleDiff) / (double)totalDiff;
}

// static u64 get_kb(const char *key) {
//     FILE *fp = fopen("/proc/meminfo", "r");
//     if (!fp) { perror("fopen"); exit(1); }

//     char line[128];
//     u64 value = 0;
//     while (fgets(line, sizeof(line), fp)) {
//         if (strncmp(line, key, strlen(key)) == 0) {
//             sscanf(line + strlen(key), ":%llu kB", &value);
//             break;
//         }
//     }
//     fclose(fp);
//     return value * 1024ULL;   /* convert KiB → bytes */
// }

void list_swaps(void) {
    FILE *fp = fopen("/proc/swaps", "r");
    if (!fp) { perror("fopen"); return; }

    char line[256];
    /* Skip header */
    fgets(line, sizeof(line), fp);
    printf("Active swap devices:\n");
    while (fgets(line, sizeof(line), fp)) {
        char dev[128];
        u64 size_kb;
        int prio;
        if (sscanf(line, "%127s %llu %d", dev, &size_kb, &prio) == 3) {
            printf("  %s – %llu MB (priority %d)\n",
                   dev, size_kb / 1024, prio);
        }
    }
    fclose(fp);
}

/**
 * @param FILE the file pointer
 * @param key the name of the info in the file we want
 */
static u64 get_mem_by_key(FILE *fp, const char *key) {
    char line[128];
    u64 value = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, key, strlen(key)) == 0) {
            sscanf(line + strlen(key), ":%llu kB", &value);
            break;
        }
    }
}

u64 kibs_to_bytes(const u64 value) {
    return value * 1024ULL;
}

void get_mem_stats(syphon_info_t *info) {

     FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp) { perror("fopen"); exit(1); }

    u64 total   = kibs_to_bytes(get_mem_by_key("MemTotal", fp));
    u64 free    = kibs_to_bytes(get_mem_by_key("MemFree", fp));
    u64 buffers = kibs_to_bytes(get_mem_by_key("Buffers", fp));
    u64 cached  = kibs_to_bytes(get_mem_by_key("Cached", fp));
    u64 swap_total = kibs_to_bytes(get_mem_by_key("SwapTotal", fp));
    u64 swap_free  = kibs_to_bytes(get_mem_by_key("SwapFree", fp));

    // char line[128];
    // u64 value = 0;
    // while (fgets(line, sizeof(line), fp)) {
    //     if (strncmp(line, key, strlen(key)) == 0) {
    //         sscanf(line + strlen(key), ":%llu kB", &value);
    //         break;
    //     }
    // }
    // fclose(fp);
    // return value * 1024ULL;   /* convert KiB → bytes */
    
    fclose(fp);

    // u64 total   = get_kb("MemTotal");
    // u64 free    = get_kb("MemFree");
    // u64 buffers = get_kb("Buffers");
    // u64 cached  = get_kb("Cached");

    u64 used = total - free - buffers - cached;

    // u64 swap_total = get_kb("SwapTotal");
    // u64 swap_free  = get_kb("SwapFree");
    u64 swap_used  = swap_total - swap_free;

    printf("Memory (bytes):\n");
    printf("  Total: %llu\n", total);
    printf("  Used : %llu\n", used);
    printf("  Free : %llu\n", free + buffers + cached);
    printf("\nSwap (bytes):\n");
    printf("  Total: %llu\n", swap_total);
    printf("  Used : %llu\n", swap_used);
    printf("  Free : %llu\n", swap_free);

    info->mem_total = total;
    info->mem_used = used;
    info->mem_free = free + buffers + cached;
    
    info->swap_total = swap_total;
    info->swap_used = swap_used;
    info->swap_free = swap_free;

}

void get_cpu_stats(syphon_info_t *info) {
    FILE *fp;
    char line[256];
    CpuStat *prev = NULL;
    size_t coreCount = 0;

    while (1) {
        fp = fopen("/proc/stat", "r");
        if (!fp) { perror("fopen"); return 1; }

        size_t idx = 0;
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, "cpu", 3) != 0) break;   // stop after cpu lines
            if (idx == 0) { /* skip the aggregate "cpu " line */ idx++; continue; }

            if (prev == NULL) {
                /* allocate on first pass */
                prev = calloc(1, sizeof(CpuStat));
                coreCount = 1;
            } else {
                prev = realloc(prev, ++coreCount * sizeof(CpuStat));
            }
            CpuStat cur;
            if (parse_cpu_line(line, &cur) == 0) {
                if (coreCount > 1) {
                    double busy = calc_busy(&prev[coreCount-2], &cur);
                    printf("Core %zu: %.1f%% busy\n", coreCount-2, busy);
                }
                prev[coreCount-2] = cur;
            }
        }
        fclose(fp);
        sleep(1);   // interval – adjust as you wish
    }
    free(prev);
}

void get_proc_stats(syphon_info_t *info) {

    /**
     * You only get the short command name (comm). If you need the full command line, you’d read /proc/<pid>/cmdline (it’s a NUL‑separated string).
        Permissions: a regular user can’t read the cmdline of processes owned by other users unless the kernel is configured with kernel.yama.ptrace_scope=0 or similar. This mirrors what tools like ps show.
        The /proc layout is stable now, but it is technically a kernel‑internal interface; future kernels could deprecate some fields (unlikely, though)
    */

    DIR *proc = opendir("/proc");
    if (!proc) {
        perror("opendir /proc");
        return 1;
    }

    struct dirent *entry;
    printf("PID   Name\n");
    while ((entry = readdir(proc)) != NULL) {
        if (!isdigit(entry->d_name[0]))
            continue;               // skip non‑numeric entries

        char commPath[256];
        snprintf(commPath, sizeof(commPath),
                 "/proc/%s/comm", entry->d_name);

        FILE *f = fopen(commPath, "r");
        if (!f)
            continue;               // may disappear between readdir and fopen

        char name[256];
        if (!fgets(name, sizeof(name), f)) { fclose(f); continue; }
        fclose(f);
        /* strip trailing newline */
        name[strcspn(name, "\n")] = '\0';

        //printf("%5s  %s\n", entry->d_name, name);
        if(info->cores_info->length >= count){
            // set the values
            syp_proc_info_t *pi = (syp_proc_info_t *)da_at(info->procs_info, i);
            if(pi){
                pi->name = name;
                pi->pid = atoi(entry->d_name);
                pi->command = "what?";
            }
        } else {
            syp_proc_info_t pi = {
                .pid=atoi(entry->d_name),
                .name=name,
                .command="what?"
            };
            //push the values
            if (!da_push(info->procs_info, &pi)) {
                fprintf(stderr, "push failed at %zu\n", i);
                da_free(info->procs_info);
                return;
            }
        }
        
        
    }
    closedir(proc);
}

void get_network_stats(syphon_info_t *info) {
    FILE *fp = fopen("/proc/net/dev", "r");
    if (!fp) { perror("fopen"); return 1; }

    char line[256];
    /* Skip the two header lines */
    fgets(line, sizeof(line), fp);
    fgets(line, sizeof(line), fp);

    printf("Network traffic (total bytes since boot):\n");
    u64 ttl_tx = 0;
    u64 ttl_rx = 0;
    while (fgets(line, sizeof(line), fp)) {
        char iface[32];
        u64 rxb, txb;
        /* Format: iface:  rxb ... txb ... */
        if (sscanf(line,
                   " %31[^:]: %llu %*s %*s %*s %*s %*s %*s %*s %llu",
                   iface, &rxb, &txb) == 3) {
            printf("%-10s  TX: %llu  RX: %llu\n",
                   iface, txb, rxb);
            ttl_tx += txb;
            ttl_rx += rxb;
        }
    }
    fclose(fp);

    info->net_total_tx = ttl_tx;
    info->net_total_rx = ttl_rx;
}

void get_device_stats(syphon_info_t *info) {
    
}


void syphon_info_get(syphon_info_t *info) {
    
    printf("GET INFO: %s", PLATFORM_LINUX ? "linux" : "?");
    
    // ---------- CPU ----------
    get_cpu_stats(info);

    // ---------- NETWORK ----------
    get_network_stats(info);

    // ---------- MEMORY ----------
    get_mem_stats(info);

    // ---------- DISK INFO ----------
    get_fs_stats("/", &info);
    //get_fs_stats("/home");
    list_swaps();
    // ---------- DEVICES ----------
    get_device_stats(info);

}
#endif