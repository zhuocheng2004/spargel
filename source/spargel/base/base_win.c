#include <spargel/base/base.h>

typedef void* HMODULE;
typedef unsigned long DWORD;
typedef char* LPSTR;

__declspec(dllimport) DWORD
    __stdcall GetModuleFileNameA(HMODULE hModule, LPSTR lpFilename,
                                 DWORD nSize);

ssize spargel_get_executable_path(char* buf, ssize buf_size) {
  return GetModuleFileNameA(NULL, buf, buf_size);
}
