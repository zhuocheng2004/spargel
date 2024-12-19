#pragma once

#include <spargel/base/vector.h>
#include <spargel/base/win_defs.h>
#include <spargel/base/win_types.h>
#include <spargel/ui/ui.h>

namespace spargel::ui {

    class window_win32;

    class platform_win32 : public platform {
        friend window_win32;

    public:
        platform_win32(HINSTANCE hInstance)
            : platform(platform_kind::win32), _hInstance(hInstance) {}

        void start_loop() override;

        base::unique_ptr<window> make_window(u32 width, u32 height) override;

    private:
        HINSTANCE _hInstance;

        base::vector<window_win32*> _windows;

        void _run_render_callbacks();
    };

    class window_win32 : public window {
        friend platform_win32;

    public:
        window_win32(platform_win32* platform, HWND hwnd);
        ~window_win32() override;

        void set_title(char const* title) override;

        window_handle handle() override;

    private:
        platform_win32* _platform;
        HWND _hwnd;
    };

    base::unique_ptr<platform> make_platform_win32();

}  // namespace spargel::ui
