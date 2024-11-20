#pragma once

#include "spargel/base/compiler.h"

namespace spargel::base {

[[noreturn]] inline void unreachable() {
#if defined(SPARGEL_COMPILER_IS_CLANG) || defined(SPARGEL_COMPILER_IS_GCC)
  __builtin_unreachable();
#elif defined(SPARGEL_COMPILER_IS_MSVC)
  __assume(false);
#endif
}

}  // namespace spargel::base
