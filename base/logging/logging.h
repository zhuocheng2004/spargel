#ifndef BASE_LOGGIN_LOGGIN_H_
#define BASE_LOGGIN_LOGGIN_H_

#include "base/meta/source_location.h"

namespace base::logging {

void Log(char const* message,
         meta::SourceLocation loc = meta::SourceLocation::Current());

}

#endif
