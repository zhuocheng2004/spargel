#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include <xcb/xcb.h>


#define IN_RANGE(x, x0, x1)     ((x) >= (x0) && (x) <= (x1))
#define CLAMP(x, x0, x1)        ((x) < (x0) ? (x0) : ((x) > (x1) ? (x1) : (x)))
#define MAKE_COLOR(r, g, b)     ((r) << 16 | (g) << 8 | (b))


static double getTimeMilliseconds() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + (double) tv.tv_usec / 1000000;
}

/**
 * Adapted from: https://github.com/Inseckto/HSV-to-RGB/blob/master/HSV2RGB.c
 * h: [0, 360], s: [0, 100], v:[0, 100] 
 */
static int colorHSV(float h, float s, float v) {
    h = h / 360; s = s / 100; v = v / 100;

    int i = floor(h * 6);
    float f = h * 6 - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);

    float r, g, b;
    switch (i % 6)
    {
        case 0: r = v, g = t, b = p; break;
        case 1: r = q, g = v, b = p; break;
        case 2: r = p, g = v, b = t; break;
        case 3: r = p, g = q, b = v; break;
        case 4: r = t, g = p, b = v; break;
        case 5: r = v, g = p, b = q; break;
    }

    return MAKE_COLOR((int) (r * 255), (int) (g * 255), (int) (b * 255));
}


int main()
{
    xcb_connection_t    *connection;
    xcb_screen_t        *screen;
    xcb_window_t        window;

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


    /* Create a black foreground graphic context */
    xcb_gcontext_t  rectangle_gc = xcb_generate_id(connection);
    uint32_t        rectangle_gc_mask         = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
    uint32_t        rectangle_gc_values[2]    = {
        screen->black_pixel, 
        0
    };
    xcb_create_gc(connection, rectangle_gc, screen->root, rectangle_gc_mask, rectangle_gc_values);


    /* Create a window */
    window = xcb_generate_id(connection);

    uint32_t mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    uint32_t values[2] = {
        screen->white_pixel,
        XCB_EVENT_MASK_EXPOSURE | 
        XCB_EVENT_MASK_STRUCTURE_NOTIFY |
        XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
        XCB_EVENT_MASK_BUTTON_1_MOTION |
        XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE
    };

    xcb_create_window(connection,
            XCB_COPY_FROM_PARENT,   /* depth*/
            window,
            screen->root,           /* parent window */
            0, 0,                   /* x, y */
            800, 600,               /* width, height */
            10,                     /* border width */
            XCB_WINDOW_CLASS_INPUT_OUTPUT,  /* class */
            screen->root_visual,    /* visual */
            mask, values);
    
    /* Map the window on the screen */
    xcb_map_window(connection, window);
    xcb_flush(connection);


    /* Graphic context for segments */
    xcb_gcontext_t  segment_gc      = xcb_generate_id(connection);
    uint32_t        segment_gc_mask         = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
    uint32_t        segment_gc_values[2]    = {
        screen->black_pixel, 
        0
    };
    xcb_create_gc(connection, segment_gc, screen->root, segment_gc_mask, segment_gc_values);

    int should_stop = 0;
    int width, height;
    int x1, y1, x2, y2;
    int drawing = 0;
    int pointer_x, pointer_y;
    xcb_generic_event_t *event;
    while (!should_stop && (event = xcb_wait_for_event(connection))) {
        switch (event->response_type & ~0x80) {
            case XCB_EXPOSE: {
                    xcb_expose_event_t *ev = (xcb_expose_event_t*) event;
                    xcb_rectangle_t rectangles[] = {
                        {
                            .x = x1, .y = y1,
                            .width = x2 - x1, .height = y2 - y1
                        },
                    };
                    xcb_poly_rectangle(connection, window, rectangle_gc, 1, rectangles);
                    xcb_flush(connection);
                }
                break;
            case XCB_CONFIGURE_NOTIFY: {
                    xcb_configure_notify_event_t *ev = (xcb_configure_notify_event_t*) event;
                    width = ev->width;
                    height = ev->height;
                    x1 = width / 8; y1 = height / 8; 
                    x2 = width * 7 / 8; y2 = height * 7 / 8;
                }
                break;
            case XCB_BUTTON_PRESS: {
                    xcb_button_press_event_t *ev = (xcb_button_press_event_t*) event;
                    if (ev->detail == 1 /* left button */) {
                        pointer_x = ev->event_x;
                        pointer_y = ev->event_y;
                        drawing = 1;
                    }
                }
                break;
            case XCB_BUTTON_RELEASE: {
                    drawing = 0;
                }
                break;
            case XCB_MOTION_NOTIFY: {
                    xcb_motion_notify_event_t *ev = (xcb_motion_notify_event_t*) event;
                    int x = ev->event_x, y = ev->event_y;
                    if (drawing && (IN_RANGE(pointer_x, x1, x2) && IN_RANGE(pointer_y, y1, y2)) || (IN_RANGE(x, x1, x2) && IN_RANGE(y, y1, y2))) {
                        double t = getTimeMilliseconds();
                        uint32_t values[1] = { colorHSV(sin(t*3) * 179 + 180, 100, 100) };
                        xcb_change_gc(connection, segment_gc, XCB_GC_FOREGROUND, values);

                        xcb_segment_t segments[] = {
                            {
                                .x1 = CLAMP(pointer_x, x1+1, x2-1), .y1 = CLAMP(pointer_y, y1+1, y2-1), 
                                .x2 = CLAMP(x, x1+1, x2-1), .y2 = CLAMP(y, y1+1, y2-1)
                            },
                        };
                        xcb_poly_segment(connection, window, segment_gc, 1, segments);
                        xcb_flush(connection);
                    }
                    pointer_x = x;
                    pointer_y = y;
                }
                break;
            case XCB_KEY_PRESS: {
                    xcb_key_press_event_t *ev = (xcb_key_press_event_t*) event;
                    if (ev->detail == 9 /*FIXME*/)
                        should_stop = 1;
                }
                break;
            default:
                break;
        }

        free(event);
    }


    xcb_disconnect(connection);

    return 0;
}
