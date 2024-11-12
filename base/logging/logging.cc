#include "base/logging/logging.h"

#include <stdio.h>

namespace base::logging {

void Log(char const* message, meta::SourceLocation loc) {
  printf("[%s:%d:%s]: %s\n", loc.FileName(), loc.Line(), loc.FunctionName(),
         message);
}

}  // namespace base::logging
