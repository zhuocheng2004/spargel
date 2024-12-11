#include <spargel/base/platform.h>
#include <spargel/base/types.h>

/* libc */
#include <errno.h>
#include <string.h>

/* platform */
#include <dlfcn.h>
#include <execinfo.h>
#include <mach-o/dyld.h>
#include <unistd.h>

namespace spargel::base {

    /**
     * > the total bufsize needed could be more than MAXPATHLEN
     */
    ssize _get_executable_path(char* buf, ssize buf_size) {
        int result = _NSGetExecutablePath(buf, (uint32_t*)&buf_size);
        if (result != 0) return 0;
        return strlen(buf);
    }

    namespace {

        void write_stderr(char const* buf, ssize len) {
            ssize_t result;
            do {
                result = write(STDERR_FILENO, buf, len);
            } while (result == -1 && errno == EINTR);
        }

        const char digits[] = "0123456789abcdef";

        void write_pointer(void* x) {
            char buf[30] = {};
            u64 n = (u64)x;
            for (int i = 0; i < 16; i++) {
                buf[15 - i] = digits[n % 16];
                n /= 16;
            }
            write_stderr(buf, 16);
        }

        bool symbolize(void* pc, char* buf, ssize size) {
            Dl_info info;
            if (dladdr(pc, &info)) {
                if (strlen(info.dli_sname) < size) {
                    strcpy(buf, info.dli_sname);
                    return true;
                }
            }
            return false;
        }

    }  // namespace

    inline constexpr ssize max_stack_traces = 128;
    inline constexpr ssize max_symbol_size = 255;

    void print_backtrace() {
        void* entries[max_stack_traces] = {};
        int count = backtrace(entries, max_stack_traces);
        char symbol_buf[max_symbol_size + 1] = {};
        for (int i = 0; i < count; i++) {
            write_stderr("  # 0x", 6);
            write_pointer(entries[i]);
            write_stderr(" ", 1);
            if (symbolize(entries[i], symbol_buf, max_symbol_size)) {
                write_stderr(symbol_buf, strlen(symbol_buf));
            } else {
                write_stderr("<unknown>", 9);
            }
            write_stderr("\n", 1);
        }
    }

}  // namespace spargel::base
