#include <spargel/base/base.h>
#include <stdlib.h>
#include <string.h>

void spargel_unreachable() {
#if defined(SPARGEL_COMPILER_IS_CLANG) || defined(SPARGEL_COMPILER_IS_GCC)
  __builtin_unreachable();
#elif defined(SPARGEL_COMPILER_IS_MSVC)
  __assume(false);
#endif
}

void sbase_unreachable() { spargel_unreachable(); }

struct sbase_string sbase_string_from_range(char const* begin,
                                            char const* end) {
  struct sbase_string str;
  str.length = end - begin;
  str.data = malloc(str.length + 1);
  memcpy(str.data, begin, str.length);
  str.data[str.length] = 0;
  return str;
}

bool sbase_string_is_equal(struct sbase_string lhs, struct sbase_string rhs) {
  if (lhs.length != rhs.length) return false;
  return memcmp(lhs.data, rhs.data, lhs.length) == 0;
}

void sbase_string_deinit(struct sbase_string str) {
  if (str.data) free(str.data);
}
