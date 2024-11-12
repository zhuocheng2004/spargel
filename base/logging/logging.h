#ifndef BASE_LOGGIN_LOGGIN_H_
#define BASE_LOGGIN_LOGGIN_H_

#include "base/meta/source_location.h"

namespace base::logging {

enum class LogLevel {
  Debug,
  Info,
  Warning,
  Error,
  Fatal,
};

void Log(LogLevel level, char const* message,
         meta::SourceLocation loc = meta::SourceLocation::Current());

}  // namespace base::logging

#endif
