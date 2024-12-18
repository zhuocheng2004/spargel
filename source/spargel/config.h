#pragma once

/*
 * Configurations can be tweaked in this file.
 */

#include <spargel/autoconfig.h>

#define SPARGEL_IS_POSIX SPARGEL_IS_ANDROID || SPARGEL_IS_LINUX || SPARGEL_IS_MACOS
