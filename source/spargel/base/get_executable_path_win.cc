#include <spargel/base/get_executable_path.h>
#include <windows.h>

namespace spargel::base {

size_t getExecutablePath(char* buf, size_t buf_size) {
    return GetModuleFileNameA(nullptr, buf, buf_size);
}

}  // namespace spargel::base
