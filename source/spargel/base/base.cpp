#include <spargel/base/base.h>
#include <stdlib.h>
#include <string.h>
#include <spargel/base/source_location.h>

// void sbase_unreachable()
// {
//     sbase_print_backtrace();
// #if defined(SPARGEL_COMPILER_IS_CLANG) || defined(SPARGEL_COMPILER_IS_GCC)
//     __builtin_unreachable();
// #elif defined(SPARGEL_COMPILER_IS_MSVC)
//     __assume(false);
// #endif
// }

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

struct sbase_string sbase_string_concat(struct sbase_string str1, struct sbase_string str2)
{
    struct sbase_string str;
    str.length = str1.length + str2.length;
    str.data = (char*)malloc(str.length + 1);
    memcpy(str.data, str1.data, str1.length);
    memcpy(str.data + str1.length, str2.data, str2.length);
    str.data[str.length] = '\0';
    return str;
}
