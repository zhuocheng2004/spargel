module;

#include <stdlib.h>
#include <string.h>

#if defined(SPARGEL_IS_MACOS)
#include <dlfcn.h>
#include <errno.h>
#include <execinfo.h>
#include <mach-o/dyld.h>
#include <string.h>
#include <unistd.h>
#endif

#if defined(SPARGEL_IS_LINUX)
#include <unistd.h>
#include <stdio.h>
#endif

#include <spargel/base/types.h>
#include <spargel/base/const.h>
#include <spargel/base/macros.h>

module spargel.base.c;

void sbase_unreachable()
{
    sbase_print_backtrace();
#if defined(SPARGEL_COMPILER_IS_CLANG) || defined(SPARGEL_COMPILER_IS_GCC)
    __builtin_unreachable();
#elif defined(SPARGEL_COMPILER_IS_MSVC)
    __assume(false);
#endif
}

struct sbase_string sbase_string_from_range(char const* begin, char const* end)
{
    struct sbase_string str;
    str.length = end - begin;
    str.data = (char*)malloc(str.length + 1);
    memcpy(str.data, begin, str.length);
    str.data[str.length] = 0;
    return str;
}

bool sbase_string_is_equal(struct sbase_string lhs, struct sbase_string rhs)
{
    if (lhs.length != rhs.length) return false;
    return memcmp(lhs.data, rhs.data, lhs.length) == 0;
}

void sbase_string_deinit(struct sbase_string str)
{
    if (str.data) free(str.data);
}

void sbase_string_copy(struct sbase_string* dst, struct sbase_string src)
{
    if (dst->data) free(dst->data);
    dst->length = src.length;
    dst->data = (char*)malloc(dst->length + 1);
    memcpy(dst->data, src.data, dst->length);
    dst->data[dst->length] = '\0';
}

struct sbase_string sbase_string_concat(struct sbase_string str1,
                                        struct sbase_string str2)
{
    struct sbase_string str;
    str.length = str1.length + str2.length;
    str.data = (char*)malloc(str.length + 1);
    memcpy(str.data, str1.data, str1.length);
    memcpy(str.data + str1.length, str2.data, str2.length);
    str.data[str.length] = '\0';
    return str;
}


ssize _spgl_get_executable_path(char* buf, ssize buf_size);

struct sbase_string spgl_get_executable_path()
{
    char* buf = (char*)malloc(PATH_MAX);
    ssize len = _spgl_get_executable_path(buf, PATH_MAX);
    if (len >= PATH_MAX) {
        buf = (char*)realloc(buf, len + 1);
        _spgl_get_executable_path(buf, len + 1);
    }
    buf[len] = '\0';
    return (struct sbase_string){
        .length = len,
        .data = buf,
    };
}

#if defined(SPARGEL_IS_MACOS)

/**
 * > the total bufsize needed could be more than MAXPATHLEN
 */
ssize _spgl_get_executable_path(char* buf, ssize buf_size)
{
    int result = _NSGetExecutablePath(buf, (uint32_t*)&buf_size);
    if (result != 0) return 0;
    return strlen(buf);
}

static void write_stderr(char const* buf, ssize len)
{
    ssize_t result;
    do {
        result = write(STDERR_FILENO, buf, len);
    } while (result == -1 && errno == EINTR);
}

static const char digits[] = "0123456789abcdef";

static void write_pointer(void* x)
{
    char buf[30] = {};
    u64 n = (u64)x;
    for (int i = 0; i < 16; i++) {
        buf[15 - i] = digits[n % 16];
        n /= 16;
    }
    write_stderr(buf, 16);
}

static bool symbolize(void* pc, char* buf, ssize size);

#define MAX_STACK_TRACES 128
#define MAX_SYMBOL_SIZE 255

void sbase_print_backtrace()
{
    void* entries[MAX_STACK_TRACES] = {};
    int count = backtrace(entries, MAX_STACK_TRACES);
    char symbol_buf[MAX_SYMBOL_SIZE + 1] = {};
    for (int i = 0; i < count; i++) {
        write_stderr("  # 0x", 6);
        write_pointer(entries[i]);
        write_stderr(" ", 1);
        if (symbolize(entries[i], symbol_buf, MAX_SYMBOL_SIZE)) {
            write_stderr(symbol_buf, strlen(symbol_buf));
        } else {
            write_stderr("<unknown>", 9);
        }
        write_stderr("\n", 1);
    }
}

static bool symbolize(void* pc, char* buf, ssize size)
{
    Dl_info info;
    if (dladdr(pc, &info)) {
        if (strlen(info.dli_sname) < size) {
            strcpy(buf, info.dli_sname);
            return true;
        }
    }
    return false;
}

#endif

#if defined(SPARGEL_IS_LINUX)
ssize _spgl_get_executable_path(char* buf, ssize buf_size)
{
    return readlink("/proc/self/exe", buf, buf_size);
}

void sbase_print_backtrace()
{
    printf("<unknown backtrace>\n");
}
#endif

#if defined(SPARGEL_IS_WINDOWS)
ssize _spgl_get_executable_path(char* buf, ssize buf_size)
{
    sbase_unreachable();
}

void sbase_print_backtrace()
{
    sbase_unreachable();
}
#endif
