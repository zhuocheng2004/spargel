#pragma once

#include <spargel/base/win_types.h>

// huge:
// #include <windows.h>

extern "C" {

WINBASEAPI DWORD WINAPI GetModuleFileNameA(HMODULE hModule, LPSTR lpFilename, DWORD nSize);
WINBASEAPI DWORD WINAPI GetModuleFileNameW(HMODULE hModule, LPWSTR lpFilename, DWORD nSize);
#ifdef UNICODE
#define GetModuleFileName GetModuleFileNameW
#else
#define GetModuleFileName GetModuleFileNameA
#endif  // !UNICODE

WINBASEAPI HMODULE WINAPI GetModuleHandleA(LPCSTR lpModuleName);
WINBASEAPI HMODULE WINAPI GetModuleHandleW(LPCWSTR lpModuleName);
#ifdef UNICODE
#define GetModuleHandle GetModuleHandleW
#else
#define GetModuleHandle GetModuleHandleA
#endif  // !UNICODE

WINBASEAPI HMODULE WINAPI LoadLibraryA(LPCSTR lpLibFileName);
WINBASEAPI BOOL WINAPI FreeLibrary(HMODULE hLibModule);
WINBASEAPI FARPROC WINAPI GetProcAddress(HMODULE hModule, LPCSTR lpProcName);

WINBASEAPI VOID WINAPI GetSystemTime(LPSYSTEMTIME lpSystemTime);

WINUSERAPI ATOM WINAPI RegisterClassA(CONST WNDCLASSA* lpWndClass);
WINUSERAPI ATOM WINAPI RegisterClassW(CONST WNDCLASSW* lpWndClass);
#ifdef UNICODE
#define RegisterClass RegisterClassW
#else
#define RegisterClass RegisterClassA
#endif  // !UNICODE

WINUSERAPI BOOL WINAPI UnregisterClassA(LPCSTR lpClassName, HINSTANCE hInstance);
WINUSERAPI BOOL WINAPI UnregisterClassW(LPCWSTR lpClassName, HINSTANCE hInstance);
#ifdef UNICODE
#define UnregisterClass UnregisterClassW
#else
#define UnregisterClass UnregisterClassA
#endif  // !UNICODE

WINUSERAPI HWND WINAPI CreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName,
                                       DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
                                       HWND hWndParent, HMENU hMenu, HINSTANCE hInstance,
                                       LPVOID lpParam);
WINUSERAPI HWND WINAPI CreateWindowExW(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
                                       DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
                                       HWND hWndParent, HMENU hMenu, HINSTANCE hInstance,
                                       LPVOID lpParam);
#ifdef UNICODE
#define CreateWindowEx CreateWindowExW
#else
#define CreateWindowEx CreateWindowExA
#endif  // !UNICODE
WINUSERAPI BOOL WINAPI DestroyWindow(HWND hWnd);
WINUSERAPI BOOL WINAPI ShowWindow(HWND hWnd, int nCmdShow);

WINUSERAPI LRESULT CALLBACK DefWindowProcA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
WINUSERAPI LRESULT CALLBACK DefWindowProcW(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

#ifdef UNICODE
#define DefWindowProc DefWindowProcW
#else
#define DefWindowProc DefWindowProcA
#endif  // !UNICODE

WINUSERAPI VOID WINAPI PostQuitMessage(int nExitCode);

WINUSERAPI BOOL WINAPI GetMessageA(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax);
WINUSERAPI BOOL WINAPI GetMessageW(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax);
#ifdef UNICODE
#define GetMessage GetMessageW
#else
#define GetMessage GetMessageA
#endif  // !UNICODE

WINUSERAPI BOOL WINAPI PeekMessageA(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax,
                                    UINT wRemoveMsg);
WINUSERAPI BOOL WINAPI PeekMessageW(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax,
                                    UINT wRemoveMsg);
#ifdef UNICODE
#define PeekMessage PeekMessageW
#else
#define PeekMessage PeekMessageA
#endif  // !UNICODE

WINUSERAPI BOOL WINAPI TranslateMessage(CONST MSG* lpMsg);

WINUSERAPI LRESULT WINAPI DispatchMessageA(CONST MSG* lpMsg);
WINUSERAPI LRESULT WINAPI DispatchMessageW(CONST MSG* lpMsg);
#ifdef UNICODE
#define DispatchMessage DispatchMessageW
#else
#define DispatchMessage DispatchMessageA
#endif  // !UNICODE

////
}
