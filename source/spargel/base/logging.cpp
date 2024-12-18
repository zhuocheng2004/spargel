#include <spargel/base/assert.h>
#include <spargel/base/logging.h>
#include <spargel/config.h>

// libc
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// platform
#if SPARGEL_IS_LINUX || SPARGEL_IS_MACOS
#include <sys/time.h>
#endif

#if SPARGEL_IS_ANDROID
#include <android/log.h>
#endif

namespace spargel::base {

    static char const* log_names[_LOG_COUNT] = {
        "DEBUG", "INFO", "WARN", "ERROR", "FATAL",
    };

    struct log_timestamp {
        int mon;
        int day;
        int hour;
        int min;
        int sec;
        int usec;
    };

    static void log_get_time(struct log_timestamp* time) {
#if SPARGEL_IS_ANDROID || SPARGEL_IS_LINUX || SPARGEL_IS_MACOS
        struct timeval tv;
        gettimeofday(&tv, NULL);
        time_t t = tv.tv_sec;
        struct tm local_time;
        localtime_r(&t, &local_time);
        struct tm* tm_time = &local_time;
        time->mon = tm_time->tm_mon + 1;
        time->day = tm_time->tm_mday;
        time->hour = tm_time->tm_hour;
        time->min = tm_time->tm_min;
        time->sec = tm_time->tm_sec;
        time->usec = tv.tv_usec;
#else
#error unimplemented
#endif
    }

    void log(int level, char const* file, char const* func, u32 line, char const* format, ...) {
        spargel_assert(level >= 0 && level < _LOG_COUNT);
        char const* name = log_names[level];
        struct log_timestamp time;
        log_get_time(&time);

#if SPARGEL_ENABLE_LOG_ANSI_COLOR
        switch (level) {
        case 0:
            fputs("\033[34m", stderr);
            break;
        case 1:
            fputs("\033[36m", stderr);
            break;
        case 2:
            fputs("\033[93m", stderr);
            break;
        case 3:
        case 4:
            fputs("\033[91m", stderr);
            break;
        default:
            fputs("\033[0m", stderr);
            break;
        }
#endif

        fprintf(stderr, "[%02d%02d/%02d%02d%02d.%06d:%s:%s:%s:%u] ", time.mon, time.day, time.hour,
                time.min, time.sec, time.usec, name, file, func, line);

        va_list ap;
        va_start(ap, format);
        vfprintf(stderr, format, ap);
        va_end(ap);

#if SPARGEL_IS_ANDROID
        int android_log_prio;
        switch (level) {
        case 0:
            android_log_prio = ANDROID_LOG_DEBUG;
            break;
        case 1:
            android_log_prio = ANDROID_LOG_INFO;
            break;
        case 2:
            android_log_prio = ANDROID_LOG_WARN;
            break;
        case 3:
            android_log_prio = ANDROID_LOG_ERROR;
            break;
        case 4:
            android_log_prio = ANDROID_LOG_FATAL;
            break;
        default:
            android_log_prio = ANDROID_LOG_UNKNOWN;
            break;
        }
        va_start(ap, format);
        __android_log_vprint(android_log_prio, "spargel", format, ap);
        va_end(ap);
#endif

#if SPARGEL_ENABLE_LOG_ANSI_COLOR
        fputs("\033[0m", stderr);
#endif

        fprintf(stderr, "\n");
    }

}  // namespace spargel::base
