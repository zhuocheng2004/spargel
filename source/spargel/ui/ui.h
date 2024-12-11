#pragma once

#include <spargel/base/base.h>

namespace spargel::ui {

    enum platform_id {
        PLATFORM_APPKIT,
        PLATFORM_WAYLAND,
        PLATFORM_WIN32,
        PLATFORM_XCB,
    };

    /**
     * @brief platform specific initialization
     */
    void init_platform();

    int platform_id();

    /**
     * @brief start the platform event loop
     */
    void platform_run();

    typedef struct window* window_id;

    /**
     * @brief create a window with given width and height
     */
    window_id create_window(int width, int height);

    /**
     * @brief destroy a window
     */
    void destroy_window(window_id window);

    /**
     * @brief set the title of a window
     */
    void window_set_title(window_id window, char const* title);

    void window_set_render_callback(window_id window, void (*render)(void*), void* data);

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

    struct window_handle window_get_handle(window_id window);

}  // namespace spargel::ui
