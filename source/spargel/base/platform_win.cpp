#include <spargel/base/base.h>

/* libc */
#include <stdio.h>

typedef void* HMODULE;
typedef unsigned long DWORD;
typedef char* LPSTR;
typedef const char* LPCSTR;

__declspec(dllimport) DWORD
    __stdcall GetModuleFileNameA(HMODULE hModule, LPSTR lpFilename, DWORD nSize);

ssize _spgl_get_executable_path(char* buf, ssize buf_size) {
    return GetModuleFileNameA(NULL, buf, buf_size);
}

void sbase_print_backtrace() { printf("<unknown backtrace>\n"); }
