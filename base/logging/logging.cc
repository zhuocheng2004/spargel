#include "base/logging/logging.h"

#include <stdio.h>

namespace base::logging {

namespace impl {

static char const* LogLevelToName(LogLevel level) {
  switch (level) {
    case LogLevel::Debug:
      return "debug";
    case LogLevel::Info:
      return "info";
    case LogLevel::Warning:
      return "warning";
    case LogLevel::Error:
      return "error";
    case LogLevel::Fatal:
      return "fatal";
  }
}

}  // namespace impl

void Log(LogLevel level, char const* message, meta::SourceLocation loc) {
  printf("[%s:%s:%d:%s] %s\n", impl::LogLevelToName(level), loc.FileName(),
         loc.Line(), loc.FunctionName(), message);
}

}  // namespace base::logging
