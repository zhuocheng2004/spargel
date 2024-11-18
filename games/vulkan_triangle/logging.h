#ifndef BASE_LOGGIN_LOGGIN_H_
#define BASE_LOGGIN_LOGGIN_H_

#include <stddef.h>
#include <stdint.h>

#define LOG(level, ...)                                                    \
  ::base::logging::LogImpl(::base::meta::SourceLocation::Current(), level, \
                           __VA_ARGS__)

#ifdef __has_builtin
#define HAS_BUILTIN(x) __has_builtin(x)
#else
#define HAS_BUILTIN(x) 0
#endif

namespace base::meta {

namespace impl {

#if HAS_BUILTIN(__remove_reference_t)
template <typename T>
using RemoveReferenceType = __remove_reference_t(T);
#else
template <typename T>
struct RemoveReference {
  using Type = T;
};
template <typename T>
struct RemoveReference<T&> {
  using Type = T;
};
template <typename T>
struct RemoveReference<T&&> {
  using Type = T;
};
template <typename T>
using RemoveReferenceType = RemoveReference<T>::Type;
#endif

#if HAS_BUILTIN(__is_lvalue_reference)
template <typename T>
bool IsLValueReference = __is_lvalue_reference(T);
#else
template <typename T>
struct IsLValueReferenceHelper {
  static constexpr bool Value = false;
};
template <typename T>
struct IsLValueReferenceHelper<T&> {
  static constexpr bool Value = true;
};
template <typename T>
bool IsLValueReference = IsLValueReferenceHelper<T>::Value;
#endif

}  // namespace impl

template <typename T>
T&& Forward(impl::RemoveReferenceType<T>& value) {
  return static_cast<T&&>(value);
}

template <typename T>
T&& Forward(impl::RemoveReferenceType<T>&& value) {
  static_assert(false);
}

}  // namespace base::meta

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
 public:
  static consteval SourceLocation Current() { return SourceLocation(); }
  constexpr uint32_t Line() const { return 0; }
  constexpr uint32_t Column() const { return 0; }
  constexpr char const* FileName() const { return "<unknown file>"; }
  constexpr char const* FunctionName() const { return "<unknown function>"; }
#endif
};

}  // namespace base::meta

namespace base::logging {

enum class LogLevel {
  Debug,
  Info,
  Warning,
  Error,
  Fatal,
};

class LogMessage {
 public:
  virtual void Write(char const*) = 0;
  virtual void Write(char) = 0;
  virtual void Write(uint32_t) = 0;
  virtual void Write(size_t) = 0;
  virtual void End() = 0;
};

class Logger {
 public:
  static Logger* Default();
  virtual LogMessage* BeginMessage(meta::SourceLocation const& loc,
                                   LogLevel level) = 0;
};

void Log(LogLevel level, char const* message,
         meta::SourceLocation loc = meta::SourceLocation::Current());

template <typename... Args>
void LogImpl(meta::SourceLocation loc, LogLevel level, Args&&... args) {
  auto message = Logger::Default()->BeginMessage(loc, level);
  (message->Write(meta::Forward<Args>(args)), ...);
  message->End();
}

}  // namespace base::logging

#endif
