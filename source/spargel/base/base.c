#include <spargel/base/base.h>

void spargel_unreachable() {
#if defined(SPARGEL_COMPILER_IS_CLANG) || defined(SPARGEL_COMPILER_IS_GCC)
  __builtin_unreachable();
#elif defined(SPARGEL_COMPILER_IS_MSVC)
  __assume(false);
#endif
}

void sbase_unreachable() { spargel_unreachable(); }
