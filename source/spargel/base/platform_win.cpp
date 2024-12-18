#include <spargel/base/base.h>

/* libc */
#include <stdio.h>

typedef void* HMODULE;
typedef unsigned long DWORD;
typedef char* LPSTR;
typedef const char* LPCSTR;

__declspec(dllimport) DWORD
    __stdcall GetModuleFileNameA(HMODULE hModule, LPSTR lpFilename, DWORD nSize);

namespace spargel::base {

    usize _get_executable_path(char* buf, usize buf_size) {
        return GetModuleFileNameA(NULL, buf, buf_size);
    }

    void print_backtrace() { printf("<unknown backtrace>\n"); }

}  // namespace spargel::base
