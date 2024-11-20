#include "spargel/base/compiler.h"
#include "spargel/base/unimplmented.h"

namespace spargel::base {

void unimplemented() {
#if defined(SPARGEL_COMPILER_IS_CLANG) || defined(SPARGEL_COMPILER_IS_GCC)
  __builtin_trap();
#elif defined(SPARGEL_COMPILER_IS_MSVC)
  __assume(false);
#endif
}

}  // namespace spargel::base
