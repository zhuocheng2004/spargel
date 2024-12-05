#pragma once

#include <spargel/base/base.h>

/**
 * @brief platform specific initialization
 */
void sui_init_platform();

/**
 * @brief start the platform event loop
 */
void sui_platform_run();

typedef struct sui_window* sui_window_id;

/**
 * @brief create a window with given width and height
 */
sui_window_id sui_create_window(int width, int height);

/**
 * @brief destroy a window
 */
void sui_destroy_window(sui_window_id window);

/**
 * @brief set the title of a window
 */
void sui_window_set_title(sui_window_id window, char const* title);

void sui_window_set_render_callback(sui_window_id window, void (*render)(void*), void* data);
