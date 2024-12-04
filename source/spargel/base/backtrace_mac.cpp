#include <spargel/base/backtrace.h>

#include <spargel/base/types.h>

#include <dlfcn.h>
#include <errno.h>
#include <execinfo.h>
#include <mach-o/dyld.h>
#include <string.h>
#include <unistd.h>

namespace spargel::base {

    namespace {

        void write_stderr(char const* buf, ssize len) {
            ssize_t result;
            do {
                result = write(STDERR_FILENO, buf, len);
            } while (result == -1 && errno == EINTR);
        }

        static const char digits[] = "0123456789abcdef";

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

    inline constexpr ssize MAX_STACK_TRACES = 128;
    inline constexpr ssize MAX_SYMBOL_SIZE = 255;

    void PrintBacktrace() {
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

}  // namespace spargel::base
