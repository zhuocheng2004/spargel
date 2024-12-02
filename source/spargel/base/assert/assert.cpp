module;

#include <errno.h>
#include <unistd.h>

#include <spargel/base/types.h>
#include <spargel/base/macros.h>

module spargel.base.assert;

import spargel.base.c;

namespace spargel::base {

namespace {
void write_stderr(char const* buf, ssize len)
{
    ssize_t result;
    do {
        result = write(STDERR_FILENO, buf, len);
    } while (result == -1 && errno == EINTR);
}
}  // namespace

void panic()
{
    write_stderr("======== PANIC ========\n", 24);
    sbase_print_backtrace();
#if defined(SPARGEL_COMPILER_IS_CLANG) || defined(SPARGEL_COMPILER_IS_GCC)
    __builtin_trap();
#elif defined(SPARGEL_COMPILER_IS_MSVC)
    __assume(false);
#endif
}

void unreachable()
{
    write_stderr("======== UNREACHABLE ========\n", 30);
    sbase_print_backtrace();
#if defined(SPARGEL_COMPILER_IS_CLANG) || defined(SPARGEL_COMPILER_IS_GCC)
    __builtin_unreachable();
#elif defined(SPARGEL_COMPILER_IS_MSVC)
    __assume(false);
#endif
}

}  // namespace spargel::base
