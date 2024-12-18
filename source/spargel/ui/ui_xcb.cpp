
#include <spargel/base/base.h>
#include <spargel/base/logging.h>
#include <spargel/ui/ui_xcb.h>

/* for clock_gettime */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

/* libc */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* xcb */
#include <xcb/xcb.h>

namespace spargel::ui {

    base::unique_ptr<platform> make_platform_xcb() { return base::make_unique<platform_xcb>(); }

    platform_xcb::platform_xcb() : platform(platform_kind::xcb) {
        /*
         * Open the connection to the X server.
         * Use the DISPLAY environment variable as the default display name.
         */
        _connection = xcb_connect(NULL, NULL);

        /* Get the first screen */
        _screen = xcb_setup_roots_iterator(xcb_get_setup(_connection)).data;

        spargel_log_debug("Information of XCB screen %d:", _screen->root);
        spargel_log_debug("  width:\t%d", _screen->width_in_pixels);
        spargel_log_debug("  height:\t%d", _screen->height_in_pixels);
        spargel_log_debug("  white pixel:\t%08x", _screen->white_pixel);
        spargel_log_debug("  black pixel:\t%08x", _screen->black_pixel);
    }

    platform_xcb::~platform_xcb() { xcb_disconnect(_connection); }

    const float FPS = 60;
    const unsigned int SECOND_NS = 1000000000U;
    const unsigned int FRAME_DELTA_NS = (unsigned int)(SECOND_NS / FPS);

    static bool in_delta(struct timespec* t1, struct timespec* t2, unsigned int delta) {
        if (t1->tv_sec == t2->tv_sec) {
            return t2->tv_nsec < t1->tv_nsec + delta;
        } else if (t1->tv_sec == t2->tv_sec - 1) {
            return t2->tv_nsec + SECOND_NS < t1->tv_nsec + delta;
        } else {
            return false;
        }
    }

    void platform_xcb::start_loop() {
        xcb_generic_event_t* event;
        struct timespec t1, t2, duration = {.tv_sec = 0, .tv_nsec = 0};
        while (true) {
            event = xcb_poll_for_event(_connection);
            if (!event) {
                clock_gettime(CLOCK_MONOTONIC, &t1);

                _run_render_callbacks();
                xcb_flush(_connection);

                /*
                 * wait for the next frame
                 * Normally the sleeping only occurs once each frame.
                 */
                while (true) {
                    clock_gettime(CLOCK_MONOTONIC, &t2);
                    if (in_delta(&t1, &t2, FRAME_DELTA_NS)) {
                        duration.tv_nsec = FRAME_DELTA_NS;
                        clock_nanosleep(CLOCK_MONOTONIC, 0, &duration, NULL);
                    } else {
                        break;
                    }
                }

                continue;
            }

            switch (event->response_type & ~0x80) {
            case XCB_EXPOSE: {
                _run_render_callbacks();
                xcb_flush(_connection);
            } break;
            case XCB_KEY_PRESS: {
                // TODO
            } break;
            default:
                break;
            }

            free(event);
        }
    }

    void platform_xcb::_run_render_callbacks() {
        for (auto& window : _windows) {
            auto delegate = window->delegate();
            if (!delegate) {
                spargel_log_fatal("window delegate not set");
                spargel_panic_here();
            }
            delegate->on_render();
        }
    }

    base::unique_ptr<window> platform_xcb::make_window(u32 width, u32 height) {
        return base::make_unique<window_xcb>(*this, width, height);
    }

    window_xcb::window_xcb(platform_xcb& platform, u32 width, u32 height) : _platform(platform) {
        _id = xcb_generate_id(platform._connection);

        uint32_t mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
        uint32_t values[2] = {platform._screen->white_pixel,
                              XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_STRUCTURE_NOTIFY};

        xcb_create_window(platform._connection, XCB_COPY_FROM_PARENT, /* depth*/
                          _id, platform._screen->root,                /* parent window */
                          0, 0,                                       /* x, y */
                          width, height,                              /* width, height */
                          10,                                         /* border width */
                          XCB_WINDOW_CLASS_INPUT_OUTPUT,              /* class */
                          platform._screen->root_visual,              /* visual */
                          mask, values);

        xcb_map_window(platform._connection, _id);
        xcb_flush(platform._connection);

        platform._windows.push(this);
    }

    window_xcb::~window_xcb() { spargel_log_debug("TODO: window_xcb::~window_xcb"); }

    void window_xcb::set_title(char const* title) {
        spargel_log_debug("TODO: window_xcb::set_title");
    }

    window_handle window_xcb::handle() {
        window_handle handle{};
        handle.xcb.connection = _platform._connection;
        handle.xcb.window = _id;
        return handle;
    }

}  // namespace spargel::ui
