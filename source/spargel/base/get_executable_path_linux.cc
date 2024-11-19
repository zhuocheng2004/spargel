#include <spargel/base/get_executable_path.h>
#include <unistd.h>

namespace spargel::base {

size_t getExecutablePath(char* buf, size_t buf_size) {
  return readlink("/proc/self/exe", buf, buf_size);
}

}  // namespace spargel::base
