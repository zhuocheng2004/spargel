#pragma once

#define CONST const
#define DECLSPEC_IMPORT __declspec(dllimport)
#define WINBASEAPI DECLSPEC_IMPORT
#define WINAPI __stdcall
#define CALLBACK __stdcall

#if !defined(_USER32_)
#define WINUSERAPI DECLSPEC_IMPORT
#else
#define WINUSERAPI extern "C"
#endif

typedef void VOID;

typedef bool BOOL;
typedef char CHAR;
typedef wchar_t WCHAR;
typedef unsigned int UINT;
typedef long LONG;
typedef unsigned short WORD;
typedef unsigned long DWORD;

#if defined(_WIN64)
typedef __int64 INT_PTR;
typedef unsigned __int64 UINT_PTR;
typedef __int64 LONG_PTR;
typedef unsigned __int64 ULONG_PTR;
#else
typedef int INT_PTR;
typedef unsigned int UINT_PTR;
typedef long LONG_PTR;
typedef unsigned long ULONG_PTR;
#endif

typedef VOID* LPVOID;
typedef CHAR* LPSTR;
typedef CONST CHAR* LPCSTR;
typedef WCHAR* LPWSTR;
typedef CONST WCHAR* LPCWSTR;

typedef INT_PTR(WINAPI* FARPROC)();

typedef LONG_PTR LRESULT;
typedef UINT_PTR WPARAM;
typedef LONG_PTR LPARAM;

typedef WORD ATOM;

typedef void* HANDLE;
typedef HANDLE HBRUSH;
typedef HANDLE HICON;
typedef HANDLE HINSTANCE;
typedef HANDLE HMENU;
typedef HANDLE HMODULE;
typedef HANDLE HMONITOR;
typedef HANDLE HWND;

typedef HICON HCURSOR;

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
    LONG x;
    LONG y;
} POINT;

typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);

typedef struct {
    UINT style;
    WNDPROC lpfnWndProc;
    int cbClsExtra;
    int cbWndExtra;
    HINSTANCE hInstance;
    HICON hIcon;
    HCURSOR hCursor;
    HBRUSH hbrBackground;
    LPCSTR lpszMenuName;
    LPCSTR lpszClassName;
} WNDCLASSA;
typedef struct {
    UINT style;
    WNDPROC lpfnWndProc;
    int cbClsExtra;
    int cbWndExtra;
    HINSTANCE hInstance;
    HICON hIcon;
    HCURSOR hCursor;
    HBRUSH hbrBackground;
    LPCWSTR lpszMenuName;
    LPCWSTR lpszClassName;
} WNDCLASSW;
#ifdef UNICODE
typedef WNDCLASSW WNDCLASS;
#else
typedef WNDCLASSA WNDCLASS;
#endif  // UNICODE

typedef struct {
    HWND hwnd;
    UINT message;
    WPARAM wParam;
    LPARAM lParam;
    DWORD time;
    POINT pt;
} MSG, *LPMSG;

typedef struct {
    DWORD nLength;
    LPVOID lpSecurityDescriptor;
    BOOL bInheritHandle;
} SECURITY_ATTRIBUTES;
