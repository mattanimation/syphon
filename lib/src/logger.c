#include "logger.h"


b8 initialize_logging(){
    // TODO: create log file
    return TRUE;
}
void shutdown_logging(){
    // TODO: cleanup logging/write queued entires
}

#if PLATFORM_APPLE
void platform_console_write(const char* message, log_level level) {
    // FATAL,ERROR,WARN,INFO,DEBUG,TRACE
    const char* colour_strings[] = {"0;41", "1;31", "1;33", "1;32", "1;34", "1;30"};
    printf("\033[%sm%s\033[0m", colour_strings[level], message);
}
#endif

#if PLATFORM_LINUX
void platform_console_write(const char* message, log_level level) { //u8 color
    // FATAL, ERROR, WARN, INFO, DEBUG, TRACE
    const char* color_strings[] = {"0;41", "1;31", "1;33", "1;32", "1;34", "1;30"};
    printf("\033[%sm%s\033[0m", color_strings[level], message);
}
#endif


#define LOGGING_MSG_LENGTH 32000

void log_output(log_level level, const char* message, ...){
    const char * level_strings[6] = {"[FATAL]: ","[ERROR]: ","[WARN]: ","[INFO]: ","[DEBUG]: ","[TRACE]: "};
    u8 is_error = level < LOG_LEVEL_WARN;

    // statically allocate a 32k char limited buffer
    char out_message[LOGGING_MSG_LENGTH];
    memset(out_message, 0, sizeof(out_message));

    // format the original message, 
    __builtin_va_list arg_ptr;
    va_start(arg_ptr, message);
    vsnprintf(out_message, LOGGING_MSG_LENGTH, message, arg_ptr);
    va_end(arg_ptr);

    char out_message_full[LOGGING_MSG_LENGTH];
    sprintf(out_message_full, "%s%s\n", level_strings[level], out_message);

    // Platform specific output
    platform_console_write(out_message_full, level);
    //printf("%s", out_message_full);

}