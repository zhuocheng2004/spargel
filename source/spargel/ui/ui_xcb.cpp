#include <spargel/base/base.h>
#include <spargel/ui/ui.h>

/* for clock_gettime */
#define _GNU_SOURCE

/* libc */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* xcb */
#include <xcb/xcb.h>

namespace spargel::ui {

    struct window {
        xcb_window_t id;

        void (*render_callback)(void*);
        void* render_data;
    };

    static int window_count;
    static struct window** windows;

    static xcb_connection_t* connection;
    static xcb_screen_t* screen;

    void init_platform() {
        /*
         * Open the connection to the X server.
         * Use the DISPLAY environment variable as the default display name.
         */
        connection = xcb_connect(NULL, NULL);

        /* Get the first screen */
        screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;

        printf("\n");
        printf("Information of screen %d: \n", screen->root);
        printf("  width:\t%d\n", screen->width_in_pixels);
        printf("  height:\t%d\n", screen->height_in_pixels);
        printf("  white pixel:\t%08x\n", screen->white_pixel);
        printf("  black pixel:\t%08x\n", screen->black_pixel);
        printf("\n");
    }

    int platform_id() { return PLATFORM_XCB; }

    static void run_render_callbacks() {
        if (window_count > 0) {
            for (int i = 0; i < window_count; i++) {
                struct window* window = windows[i];
                if (window->render_callback) {
                    window->render_callback(window->render_data);
                }
            }
        }
    }

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

    void platform_run() {
        int should_stop = 0;
        xcb_generic_event_t* event;
        struct timespec t1, t2, duration = {.tv_sec = 0, .tv_nsec = 0};
        while (!should_stop) {
            event = xcb_poll_for_event(connection);
            if (!event) {
                clock_gettime(CLOCK_MONOTONIC, &t1);

                run_render_callbacks();

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
                run_render_callbacks();
                xcb_flush(connection);
            } break;
            case XCB_KEY_PRESS: {
                xcb_key_press_event_t* ev = (xcb_key_press_event_t*)event;
                if (ev->detail == 9 /*FIXME*/) should_stop = 1;
            } break;
            default:
                break;
            }

            free(event);
        }

        for (int i = 0; i < window_count; i++) {
            free(windows[i]);
        }
        if (window_count > 0) {
            free(windows);
        }
        window_count = 0;
        windows = NULL;

        xcb_disconnect(connection);
    }

    window_id create_window(int width, int height) {
        xcb_window_t id = xcb_generate_id(connection);

        uint32_t mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
        uint32_t values[2] = {screen->white_pixel,
                              XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_STRUCTURE_NOTIFY |
                                  XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
                                  XCB_EVENT_MASK_BUTTON_1_MOTION | XCB_EVENT_MASK_KEY_PRESS |
                                  XCB_EVENT_MASK_KEY_RELEASE};

        xcb_create_window(connection, XCB_COPY_FROM_PARENT, /* depth*/
                          id, screen->root,                 /* parent window */
                          0, 0,                             /* x, y */
                          width, height,                    /* width, height */
                          10,                               /* border width */
                          XCB_WINDOW_CLASS_INPUT_OUTPUT,    /* class */
                          screen->root_visual,              /* visual */
                          mask, values);

        xcb_map_window(connection, id);
        xcb_flush(connection);

        window_id window = (window_id)malloc(sizeof(struct window));
        if (!window) return NULL;
        window->render_callback = NULL;
        window->render_data = NULL;

        window->id = id;

        if (window_count == 0) windows = (struct window**)malloc(sizeof(struct window*));
        window_count++;
        windows = (struct window**)realloc(windows, window_count * sizeof(struct window*));
        windows[window_count - 1] = window;

        return window;
    }

    void destroy_window(window_id window) { puts("TODO: destroy_window\n"); }

    void window_set_title(window_id window, char const* title) {
        puts("TODO: window_set_title\n");
    }

    void window_set_render_callback(window_id window, void (*render)(void*), void* data) {
        window->render_callback = render;
        window->render_data = data;
    }

    struct window_handle window_get_handle(window_id window) {
        struct window_handle handle;
        handle.xcb.connection = connection;
        handle.xcb.window = window->id;
        return handle;
    }

}  // namespace spargel::ui
