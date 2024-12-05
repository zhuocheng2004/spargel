#pragma once

#include <spargel/base/base.h>

#if __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif

enum sui_platform_id {
    SUI_PLATFORM_APPKIT,
    SUI_PLATFORM_WAYLAND,
    SUI_PLATFORM_WIN32,
    SUI_PLATFORM_XCB,
};

/**
 * @brief platform specific initialization
 */
EXTERN_C void sui_init_platform();

int sui_platform_id();

/**
 * @brief start the platform event loop
 */
EXTERN_C void sui_platform_run();

typedef struct sui_window* sui_window_id;

/**
 * @brief create a window with given width and height
 */
EXTERN_C sui_window_id sui_create_window(int width, int height);

/**
 * @brief destroy a window
 */
void sui_destroy_window(sui_window_id window);

/**
 * @brief set the title of a window
 */
EXTERN_C void sui_window_set_title(sui_window_id window, char const* title);

EXTERN_C void sui_window_set_render_callback(sui_window_id window, void (*render)(void*),
                                             void* data);
