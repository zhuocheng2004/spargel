#pragma once

#include <spargel/base/base.h>
#include <spargel/base/unique_ptr.h>

namespace spargel::ui {

    enum class platform_kind {
        android,
        appkit,
        uikit,
        wayland,
        win32,
        xcb,
    };

    struct window_handle {
        union {
            struct {
                void* layer;
            } apple;
            struct {
                void* display;
                void* surface;
            } wayland;
            struct {
                void* connection;
                int window;
            } xcb;
            struct {
                void* hwnd;
            } win32;
        };
    };

    class window_delegate {
    public:
        virtual ~window_delegate() = default;

        virtual void render() = 0;
    };

    class window {
    public:
        virtual ~window() = default;

        void set_delegate(window_delegate* delegate) { _delegate = delegate; }
        window_delegate* delegate() { return _delegate; }

        virtual void set_title(char const* title) = 0;

        // todo: improve
        virtual window_handle handle() = 0;

    private:
        window_delegate* _delegate = nullptr;
    };

    class platform {
    public:
        virtual ~platform() = default;

        platform_kind kind() const { return _kind; }

        virtual void start_loop() = 0;

        virtual base::unique_ptr<window> make_window(int width, int height) = 0;

    protected:
        explicit platform(platform_kind k) : _kind{k} {}

    private:
        platform_kind _kind;
    };

    base::unique_ptr<platform> make_platform();

}  // namespace spargel::ui
