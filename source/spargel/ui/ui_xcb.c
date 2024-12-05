#include <spargel/base/base.h>
#include <spargel/ui/ui.h>

/* libc */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* xcb */
#include <xcb/xcb.h>

struct sui_window {
    xcb_window_t id;

    void (*render_callback)(void*);
    void* render_data;
};

static int window_count;
static struct sui_window** windows;

static xcb_connection_t* connection;
static xcb_screen_t* screen;

void sui_init_platform() {
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

void sui_platform_run() {
    int should_stop = 0;
    xcb_generic_event_t* event;
    while (!should_stop && (event = xcb_wait_for_event(connection))) {
        switch (event->response_type & ~0x80) {
        case XCB_EXPOSE: {
            if (window_count > 0) {
                for (int i = 0; i < window_count; i++) {
                    struct sui_window* window = windows[i];
                    if (window->render_callback) {
                        window->render_callback(window->render_data);
                    }
                }
            }
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

sui_window_id sui_create_window(int width, int height) {
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

    sui_window_id window = malloc(sizeof(struct sui_window));
    if (!window) return NULL;
    window->render_callback = NULL;
    window->render_data = NULL;

    window->id = id;

    if (window_count == 0) windows = (struct sui_window**)malloc(sizeof(struct sui_window*));
    window_count++;
    windows = (struct sui_window**)realloc(windows, window_count * sizeof(struct sui_window*));
    windows[window_count - 1] = window;

    return window;
}

void sui_destroy_window(sui_window_id window) { puts("TODO: sui_destroy_window\n"); }

void ui_window_set_title(sui_window_id window, char const* title) {
    puts("TODO: sui_window_set_title\n");
}

void sui_window_set_render_callback(sui_window_id window, void (*render)(void*), void* data) {
    window->render_callback = render;
    window->render_data = data;
}
