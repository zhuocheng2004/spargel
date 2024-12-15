#pragma once

#include <spargel/base/base.h>
#include <spargel/base/span.h>
#include <spargel/base/unique_ptr.h>
#include <spargel/base/vector.h>

namespace spargel::ui {

    enum class platform_kind {
        android,
        appkit,
        uikit,
        wayland,
        win32,
        xcb,
    };

    /// the native handle of a window
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

    enum keyboard_action {
        press,
        release,
    };

    enum physical_key {
        escape,
        key_a,
        key_b,
        key_c,
        space,
    };

    struct keyboard_event {
        int key;
    };

    class window_delegate {
    public:
        virtual ~window_delegate() = default;

        /// @brief the window is requested to draw the contents
        virtual void on_render() {}

        /// @brief the window is requested to close
        virtual void on_close_requested() {}

        /// @brief the window is closed
        virtual void on_closed() {}

        /// @brief a keyboard event is received
        virtual void on_keyboard(keyboard_event& e) {}
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

    class view {
    public:
        view();
        virtual ~view();

        view* parent() { return _parent; }
        base::span<view*> children();

        void add_child(view* v);

    private:
        view* _parent = nullptr;
        base::vector<view*> _children;
    };

}  // namespace spargel::ui
