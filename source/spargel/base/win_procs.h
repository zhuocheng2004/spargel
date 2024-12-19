#pragma once

#include <spargel/base/win_types.h>

// huge:
// #include <windows.h>

extern "C" {

WINBASEAPI DWORD WINAPI GetModuleFileNameA(HMODULE hModule, LPSTR lpFilename, DWORD nSize);
WINBASEAPI HMODULE WINAPI LoadLibraryA(LPCSTR lpLibFileName);
WINBASEAPI BOOL WINAPI FreeLibrary(HMODULE hLibModule);
WINBASEAPI FARPROC WINAPI GetProcAddress(HMODULE hModule, LPCSTR lpProcName);

WINBASEAPI VOID WINAPI GetSystemTime(LPSYSTEMTIME lpSystemTime);

////
}
