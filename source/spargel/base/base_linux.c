#include <spargel/base/base.h>
#include <unistd.h>

ssize spargel_get_executable_path(char* buf, ssize buf_size) {
  return readlink("/proc/self/exe", buf, buf_size);
}
