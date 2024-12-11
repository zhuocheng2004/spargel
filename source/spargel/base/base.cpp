#include <spargel/base/base.h>

/* libc */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* platform */
#if SPARGEL_IS_LINUX || SPARGEL_IS_MACOS
#include <sys/time.h>
#endif

namespace spargel::base {

    /* there is no need to report allocation or check memory leaks if the program panics */
    void panic_at(char const* file, char const* func, ssize line) {
        /* todo: rewrite fprintf */
        fprintf(stderr, "======== PANIC [%s:%s:%ld] ========\n", file, func, line);
        print_backtrace();
#if defined(SPARGEL_COMPILER_IS_CLANG) || defined(SPARGEL_COMPILER_IS_GCC)
        __builtin_trap();
#elif defined(SPARGEL_COMPILER_IS_MSVC)
        __assume(false);
#else
#error unimplemented
#endif
    }

    void panic() { panic_at("<unknown>", "<unknown>", 0); }

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
#if SPARGEL_IS_LINUX || SPARGEL_IS_MACOS
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

    void log(int level, char const* file, char const* func, ssize line, char const* format, ...) {
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

        fprintf(stderr, "[%02d%02d/%02d%02d%02d.%06d:%s:%s:%s:%ld] ", time.mon, time.day, time.hour,
                time.min, time.sec, time.usec, name, file, func, line);

        va_list ap;
        va_start(ap, format);
        vfprintf(stderr, format, ap);
        va_end(ap);

#if SPARGEL_ENABLE_LOG_ANSI_COLOR
        fputs("\033[0m", stderr);
#endif

        fprintf(stderr, "\n");
    }

    struct alloc_stat {
        ssize current;
        ssize total;
    };

    static struct alloc_stat alloc_stats[_ALLOCATION_COUNT] = {};
    static char const* alloc_names[_ALLOCATION_COUNT] = {
        "/spargel/base", "/spargel/codec", "/spargel/ecs", "/spargel/gpu", "/spargel/ui",
    };

    void* allocate(ssize size, int tag) {
        spargel_assert(tag >= 0 && tag < _ALLOCATION_COUNT);
        void* ptr = malloc(size);
        alloc_stats[tag].current += size;
        alloc_stats[tag].total += size;
#if SPARGEL_TRACE_ALLOCATION
        log_info("allocated %ld bytes with tag %s", size, alloc_names[tag]);
#endif
        return ptr;
    }

    void* reallocate(void* ptr, ssize old_size, ssize new_size, int tag) {
        spargel_assert(tag >= 0 && tag < _ALLOCATION_COUNT);
        void* new_ptr = realloc(ptr, new_size);
        alloc_stats[tag].current += new_size - old_size;
        if (new_size > old_size) {
            alloc_stats[tag].total += new_size - old_size;
        }
        spargel_assert(alloc_stats[tag].current >= 0);
#if SPARGEL_TRACE_ALLOCATION
        log_info("reallocated %ld -> %ld bytes with tag %s", old_size, new_size, alloc_names[tag]);
#endif
        return new_ptr;
    }

    void deallocate(void* ptr, ssize size, int tag) {
        spargel_assert(tag >= 0 && tag < _ALLOCATION_COUNT);
        free(ptr);
        alloc_stats[tag].current -= size;
        spargel_assert(alloc_stats[tag].current >= 0);
#if SPARGEL_TRACE_ALLOCATION
        log_info("deallocated %ld bytes with tag %s", size, alloc_names[tag]);
#endif
    }

    void report_allocation() {
        fprintf(stderr, "======== Allocation Report ========\n");
        for (int i = 0; i < _ALLOCATION_COUNT; i++) {
            fprintf(stderr, "  %s [ current = %ld bytes, total = %ld bytes ]\n", alloc_names[i],
                    alloc_stats[i].current, alloc_stats[i].total);
        }
    }

    void check_leak() {
        for (int i = 0; i < _ALLOCATION_COUNT; i++) {
            if (alloc_stats[i].current != 0) {
                spargel_log_error("leak detected in %s", alloc_names[i]);
            }
        }
    }

    struct string string_from_range(char const* begin, char const* end) {
        spargel_assert(begin <= end);
        spargel_assert(begin != NULL);

        struct string str;
        str.length = end - begin;
        str.data = (char*)allocate(str.length + 1, ALLOCATION_BASE);
        memcpy(str.data, begin, str.length);
        str.data[str.length] = 0;
        return str;
    }

    bool string_is_equal(struct string lhs, struct string rhs) {
        if (lhs.length != rhs.length) return false;
        return memcmp(lhs.data, rhs.data, lhs.length) == 0;
    }

    void string_deinit(struct string str) {
        spargel_log_debug("str length = %ld", str.length);
        if (str.data) deallocate(str.data, str.length + 1, ALLOCATION_BASE);
    }

    void string_copy(struct string* dst, struct string src) {
        spargel_assert(src.data != NULL);

        if (dst->data) deallocate(dst->data, dst->length + 1, ALLOCATION_BASE);
        dst->length = src.length;
        dst->data = (char*)allocate(dst->length + 1, ALLOCATION_BASE);
        memcpy(dst->data, src.data, dst->length);
        dst->data[dst->length] = '\0';
    }

    struct string string_concat(struct string str1, struct string str2) {
        spargel_assert(str1.data != NULL);
        spargel_assert(str2.data != NULL);

        struct string str;
        str.length = str1.length + str2.length;
        str.data = (char*)allocate(str.length + 1, ALLOCATION_BASE);
        memcpy(str.data, str1.data, str1.length);
        memcpy(str.data + str1.length, str2.data, str2.length);
        str.data[str.length] = '\0';
        return str;
    }

}  // namespace spargel::base
