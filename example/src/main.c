#include <stdio.h>
#include <unistd.h> // for sleep

#include <syphon.h>


unsigned int running = 1;

int main(int argc, char* argv[]) {
    printf("--- starting ----\n");
    
    // create a syphon info struct
    syphon_info_t info = syphon_info_create();

    while(running) {    
        syphon_info_get(&info);
        // can access data here
        printf("coreCount: %u", info.core_count);
        printf("free RAM (bytes): %llu", info.mem_free);
        // optionally log to stdout
        syphon_info_print(&info);
        sleep(1);
    }
    // dont forget to cleanup
    syphon_info_cleanup(&info);

    printf("--- end ----\n");
    return 0;
}