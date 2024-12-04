#pragma once

#include <spargel/base/panic.h>

#define SPARGEL_ENABLE_ASSERT 1

#if SPARGEL_ENABLE_ASSERT
#define ASSERT(cond) ((cond) ? static_cast<void>(0) : spargel::base::Panic())
#else
#define ASSERT(cond)
#endif
