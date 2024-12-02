#pragma once

#include <spargel/base/base.h>

/**
 * @brief platform specific initialization
 */
void spargel_ui_init_platform();

/**
 * @brief start the platform event loop
 */
void spargel_ui_platform_run();

typedef struct spargel_ui_window* spargel_ui_window_id;

/**
 * @brief create a window with given width and height
 */
spargel_ui_window_id spargel_ui_create_window(int width, int height);

/**
 * @brief destroy a window
 */
void spargel_ui_destroy_window(spargel_ui_window_id window);

/**
 * @brief set the title of a window
 */
void spargel_ui_window_set_title(spargel_ui_window_id window,
                                 char const* title);

void spargel_ui_window_set_render_callback(spargel_ui_window_id window,
                                           void (*render)(void*), void* data);
