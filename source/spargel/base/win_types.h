#pragma once

#define DECLSPEC_IMPORT __declspec(dllimport)
#define WINBASEAPI DECLSPEC_IMPORT
#define WINAPI __stdcall

typedef void VOID;

typedef bool BOOL;
typedef unsigned short WORD;
typedef unsigned long DWORD;

#if defined(_WIN64)
typedef __int64 INT_PTR;
#else
typedef int INT_PTR;
#endif

typedef void* LPVOID;
typedef char* LPSTR;
typedef const char* LPCSTR;
typedef const wchar_t* LPCWSTR;

typedef INT_PTR(WINAPI* FARPROC)();

typedef void* HANDLE;
typedef HANDLE HINSTANCE;
typedef HANDLE HMODULE;
typedef HANDLE HMONITOR;
typedef HANDLE HWND;

typedef struct {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME, *LPSYSTEMTIME;

typedef struct {
    DWORD nLength;
    LPVOID lpSecurityDescriptor;
    BOOL bInheritHandle;
} SECURITY_ATTRIBUTES;
