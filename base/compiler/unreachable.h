#ifndef BASE_COMPILER_UNREACHABLE_H_
#define BASE_COMPILER_UNREACHABLE_H_

#include "build/build_config.h"

#if defined(COMPILER_MSVC)
#define UNREACHABLE() __assume(false)
#elif defined(COMPILER_CLANG) || defined(COMPILER_GCC)
#define UNREACHABLE() __builtin_unreachable()
#endif

#endif
