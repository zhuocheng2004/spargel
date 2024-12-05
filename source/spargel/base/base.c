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

/* there is no need to report allocation or check memory leaks if the program panics */
void sbase_panic_at(char const* file, char const* func, ssize line) {
    /* todo: rewrite fprintf */
    fprintf(stderr, "======== PANIC [%s:%s:%ld] ========\n", file, func, line);
    sbase_print_backtrace();
#if defined(SPARGEL_COMPILER_IS_CLANG) || defined(SPARGEL_COMPILER_IS_GCC)
    __builtin_trap();
#elif defined(SPARGEL_COMPILER_IS_MSVC)
    __assume(false);
#else
#error unimplemented
#endif
}

void sbase_panic() { sbase_panic_at("<unknown>", "<unknown>", 0); }

static char const* sbase_log_names[_SBASE_LOG_COUNT] = {
    "DEBUG", "INFO", "WARN", "ERROR", "FATAL",
};

struct sbase_log_timestamp {
    int mon;
    int day;
    int hour;
    int min;
    int sec;
    int usec;
};

static void sbase_log_get_time(struct sbase_log_timestamp* time) {
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

void sbase_log(int level, char const* file, char const* func, ssize line, char const* format, ...) {
    DCHECK(level >= 0 && level < _SBASE_LOG_COUNT);
    char const* name = sbase_log_names[level];
    struct sbase_log_timestamp time;
    sbase_log_get_time(&time);
    fprintf(stderr, "[%02d%02d/%02d%02d%02d.%06d:%s:%s:%s:%ld] ", time.mon, time.day, time.hour,
            time.min, time.sec, time.usec, name, file, func, line);

    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
    fprintf(stderr, "\n");
}

struct sbase_alloc_stat {
    ssize current;
    ssize total;
};

static struct sbase_alloc_stat sbase_alloc_stats[_SBASE_ALLOCATION_COUNT] = {};
static char const* sbase_alloc_names[_SBASE_ALLOCATION_COUNT] = {
    "/spargel/base", "/spargel/codec", "/spargel/ecs", "/spargel/gpu", "/spargel/ui",
};

void* sbase_allocate(ssize size, int tag) {
    CHECK(tag >= 0 && tag < _SBASE_ALLOCATION_COUNT);
    void* ptr = malloc(size);
    sbase_alloc_stats[tag].current += size;
    sbase_alloc_stats[tag].total += size;
#if SPARGEL_TRACE_ALLOCATION
    sbase_log_info("allocated %ld bytes with tag %s", size, sbase_alloc_names[tag]);
#endif
    return ptr;
}

void* sbase_reallocate(void* ptr, ssize old_size, ssize new_size, int tag) {
    CHECK(tag >= 0 && tag < _SBASE_ALLOCATION_COUNT);
    void* new_ptr = realloc(ptr, new_size);
    sbase_alloc_stats[tag].current += new_size - old_size;
    if (new_size > old_size) {
        sbase_alloc_stats[tag].total += new_size - old_size;
    }
    CHECK(sbase_alloc_stats[tag].current >= 0);
#if SPARGEL_TRACE_ALLOCATION
    sbase_log_info("reallocated %ld -> %ld bytes with tag %s", old_size, new_size,
                   sbase_alloc_names[tag]);
#endif
    return new_ptr;
}

void sbase_deallocate(void* ptr, ssize size, int tag) {
    CHECK(tag >= 0 && tag < _SBASE_ALLOCATION_COUNT);
    free(ptr);
    sbase_alloc_stats[tag].current -= size;
    CHECK(sbase_alloc_stats[tag].current >= 0);
#if SPARGEL_TRACE_ALLOCATION
    sbase_log_info("deallocated %ld bytes with tag %s", size, sbase_alloc_names[tag]);
#endif
}

void sbase_report_allocation() {
    fprintf(stderr, "======== Allocation Report ========\n");
    for (int i = 0; i < _SBASE_ALLOCATION_COUNT; i++) {
        fprintf(stderr, "  %s [ current = %ld bytes, total = %ld bytes ]\n", sbase_alloc_names[i],
                sbase_alloc_stats[i].current, sbase_alloc_stats[i].total);
    }
}

void sbase_check_leak() {
    for (int i = 0; i < _SBASE_ALLOCATION_COUNT; i++) {
        if (sbase_alloc_stats[i].current != 0) {
            sbase_log_error("leak detected in %s", sbase_alloc_names[i]);
        }
    }
}

struct sbase_string sbase_string_from_range(char const* begin, char const* end) {
    CHECK(begin <= end);
    CHECK(begin != NULL);

    struct sbase_string str;
    str.length = end - begin;
    str.data = sbase_allocate(str.length + 1, SBASE_ALLOCATION_BASE);
    memcpy(str.data, begin, str.length);
    str.data[str.length] = 0;
    return str;
}

bool sbase_string_is_equal(struct sbase_string lhs, struct sbase_string rhs) {
    if (lhs.length != rhs.length) return false;
    return memcmp(lhs.data, rhs.data, lhs.length) == 0;
}

void sbase_string_deinit(struct sbase_string str) {
    sbase_log_debug("str length = %ld", str.length);
    if (str.data) sbase_deallocate(str.data, str.length + 1, SBASE_ALLOCATION_BASE);
}

void sbase_string_copy(struct sbase_string* dst, struct sbase_string src) {
    CHECK(src.data != NULL);

    if (dst->data) sbase_deallocate(dst->data, dst->length + 1, SBASE_ALLOCATION_BASE);
    dst->length = src.length;
    dst->data = sbase_allocate(dst->length + 1, SBASE_ALLOCATION_BASE);
    memcpy(dst->data, src.data, dst->length);
    dst->data[dst->length] = '\0';
}

struct sbase_string sbase_string_concat(struct sbase_string str1, struct sbase_string str2) {
    CHECK(str1.data != NULL);
    CHECK(str2.data != NULL);

    struct sbase_string str;
    str.length = str1.length + str2.length;
    str.data = sbase_allocate(str.length + 1, SBASE_ALLOCATION_BASE);
    memcpy(str.data, str1.data, str1.length);
    memcpy(str.data + str1.length, str2.data, str2.length);
    str.data[str.length] = '\0';
    return str;
}
