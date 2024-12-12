#pragma once

#include <spargel/base/base.h>

#if SPARGEL_ENABLE_ASSERT
#define spargel_assert(cond) ((cond) ? (void)(0) : spargel_panic_here())
#else
#define spargel_assert(cond)
#endif
