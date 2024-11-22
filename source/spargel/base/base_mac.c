#include <spargel/base/base.h>
#include <string.h>

/**
 * > the total bufsize needed could be more than MAXPATHLEN
 */
ssize spargel_get_executable_path(char* buf, ssize buf_size) {
  int result = _NSGetExecutablePath(buf, &buf_size);
  if (result != 0) return 0;
  return strlen(buf);
}
