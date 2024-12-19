
#include <spargel/base/logging.h>
#include <spargel/base/win_procs.h>
#include <spargel/ui/ui_win32.h>

namespace spargel::ui {

    static const char CLASS_NAME[] = "Spargel Engine Window Class";

    static LRESULT CALLBACK window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }
        return DefWindowProcA(hwnd, uMsg, wParam, lParam);
    }

    void platform_win32::start_loop() {
        MSG msg = {};

        while (true) {
            while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                    return;
                } else {
                    TranslateMessage(&msg);
                    DispatchMessageW(&msg);
                }
            }

            _run_render_callbacks();
        }
    }

    void platform_win32::_run_render_callbacks() {
        for (auto& window : _windows) {
            auto delegate = window->delegate();
            if (!delegate) {
                spargel_log_fatal("window delegate not set");
                spargel_panic_here();
            }
            delegate->on_render();
        }
    }

    base::unique_ptr<window> platform_win32::make_window(u32 width, u32 height) {
        WNDCLASS wc = {};

        wc.lpfnWndProc = window_proc;
        wc.hInstance = _hInstance;
        wc.lpszClassName = CLASS_NAME;

        RegisterClassA(&wc);

        HWND hwnd = CreateWindowExA(0,                    // optional window styles
                                    CLASS_NAME,           // window class
                                    "Spargel Engine",     // window text
                                    WS_OVERLAPPEDWINDOW,  // window style

                                    // size and position
                                    CW_USEDEFAULT, CW_USEDEFAULT, (int)width, (int)height,

                                    NULL,        // parent window
                                    NULL,        // menu
                                    _hInstance,  // instance handle
                                    NULL         // additional application data
        );

        if (hwnd == NULL) {
            spargel_log_fatal("CreateWindowEx failed");
            spargel_panic_here();
        }

        ShowWindow(hwnd, SW_SHOWNORMAL);

        return base::make_unique<window_win32>(this, hwnd);
    }

    window_win32::window_win32(platform_win32* platform, HWND hwnd)
        : _platform(platform), _hwnd(hwnd) {
        platform->_windows.push(this);
    }

    window_win32::~window_win32() { DestroyWindow(_hwnd); }

    void window_win32::set_title(char const* title) {}

    window_handle window_win32::handle() {
        window_handle handle{};
        handle.win32.hinstance = _platform->_hInstance;
        handle.win32.hwnd = _hwnd;
        return handle;
    }

    base::unique_ptr<platform> make_platform_win32() {
        HINSTANCE hInstance = (HINSTANCE)GetModuleHandleA(NULL);
        return base::make_unique<platform_win32>(hInstance);
    }

}  // namespace spargel::ui
