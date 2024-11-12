#ifndef BASE_META_SOURCE_LOCATION_H_
#define BASE_META_SOURCE_LOCATION_H_

#include <stdint.h>

#include "base/compiler/has_builtin.h"

#if HAS_BUILTIN(__builtin_source_location)
namespace std {
// Extracted from libcxx.
struct source_location {
  // The names source_location::__impl, _M_file_name, _M_function_name, _M_line,
  // and _M_column are hard-coded in the compiler and must not be changed here.
  struct __impl {
    const char* _M_file_name;
    const char* _M_function_name;
    unsigned _M_line;
    unsigned _M_column;
  };
};
}  // namespace std
#endif

namespace base::meta {

class SourceLocation {
#if HAS_BUILTIN(__builtin_source_location)
 public:
  static consteval SourceLocation Current(
      std::source_location::__impl const* ptr = __builtin_source_location()) {
    return SourceLocation(ptr);
  }
  constexpr uint32_t Line() const {
    if (ptr_) {
      return ptr_->_M_line;
    } else {
      return 0;
    }
  }
  constexpr uint32_t Column() const {
    if (ptr_) {
      return ptr_->_M_column;
    } else {
      return 0;
    }
  }
  constexpr char const* FileName() const {
    if (ptr_) {
      return ptr_->_M_file_name;
    } else {
      return "<unknown file>";
    }
  }
  constexpr char const* FunctionName() const {
    if (ptr_) {
      return ptr_->_M_function_name;
    } else {
      return "<unknown function>";
    }
  }
 private:
  constexpr explicit SourceLocation(std::source_location::__impl const* ptr)
      : ptr_{ptr} {}
  std::source_location::__impl const* ptr_;
#else
#error "unimplemented: SourceLocation"
#endif
};

}  // namespace base::meta

#endif
