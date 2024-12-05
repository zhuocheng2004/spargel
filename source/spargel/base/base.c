#include <spargel/base/base.h>

/* libc */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* platform */
#if defined(SPARGEL_IS_LINUX) || defined(SPARGEL_IS_MACOS)
#include <sys/time.h>
#endif

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

static char const* sbase_log_names[] = {
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL",
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
#if defined(SPARGEL_IS_LINUX) || defined(SPARGEL_IS_MACOS)
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
    DCHECK(level >= 0 && level <= 4);
    char const* name = sbase_log_names[level];
    struct sbase_log_timestamp time;
    sbase_log_get_time(&time);
    fprintf(stderr, "[%02d%02d/%02d%02d%02d.%06d:%s:%s:%s:%ld] ", time.mon, time.day, time.hour, time.min, time.sec, time.usec, name, file, func, line);

    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
    fprintf(stderr, "\n");
}

struct sbase_string sbase_string_from_range(char const* begin, char const* end) {
    CHECK(begin <= end);
    CHECK(begin != NULL);

    struct sbase_string str;
    str.length = end - begin;
    str.data = malloc(str.length + 1);
    memcpy(str.data, begin, str.length);
    str.data[str.length] = 0;
    return str;
}

bool sbase_string_is_equal(struct sbase_string lhs, struct sbase_string rhs) {
    if (lhs.length != rhs.length) return false;
    return memcmp(lhs.data, rhs.data, lhs.length) == 0;
}

void sbase_string_deinit(struct sbase_string str) {
    if (str.data) free(str.data);
}

void sbase_string_copy(struct sbase_string* dst, struct sbase_string src) {
    CHECK(src.data != NULL);

    if (dst->data) free(dst->data);
    dst->length = src.length;
    dst->data = malloc(dst->length + 1);
    memcpy(dst->data, src.data, dst->length);
    dst->data[dst->length] = '\0';
}

struct sbase_string sbase_string_concat(struct sbase_string str1, struct sbase_string str2) {
    CHECK(str1.data != NULL);
    CHECK(str2.data != NULL);

    struct sbase_string str;
    str.length = str1.length + str2.length;
    str.data = malloc(str.length + 1);
    memcpy(str.data, str1.data, str1.length);
    memcpy(str.data + str1.length, str2.data, str2.length);
    str.data[str.length] = '\0';
    return str;
}
