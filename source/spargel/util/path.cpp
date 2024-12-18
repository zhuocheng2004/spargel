
#include <spargel/base/const.h>
#include <spargel/util/path.h>

namespace spargel::util {

    base::string dirname(const base::string& path) {
        if (path.length() == 0) return path;

        const char* data = path.data();

        char const* cur = path.end() - 1;
        // path.length() > 0, so path.begin() != nullptr
        for (; cur >= path.begin(); cur--) {
            if (*cur == PATH_SPLIT) break;
        }

        return base::string_from_range(data, cur);
    }

}  // namespace spargel::util
