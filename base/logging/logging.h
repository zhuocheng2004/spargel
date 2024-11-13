#ifndef BASE_LOGGIN_LOGGIN_H_
#define BASE_LOGGIN_LOGGIN_H_

#include <stddef.h>
#include <stdint.h>

#include "base/meta/forward.h"
#include "base/meta/source_location.h"

#define LOG(level, ...)                                                    \
  ::base::logging::LogImpl(::base::meta::SourceLocation::Current(), level, \
                           __VA_ARGS__)

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
