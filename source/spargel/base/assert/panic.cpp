#include <spargel/base/assert/panic.h>
#include <spargel/base/base.h>
#include <errno.h>
#include <unistd.h>

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

}  // namespace spargel::base
