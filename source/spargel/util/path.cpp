
#include <spargel/base/const.h>
#include <spargel/util/path.h>

namespace spargel::util {

    base::string dirname(const base::string& path) {
        if (path.length() == 0) return path;

        const char* data = path.data();

        int end;
        for (end = path.length() - 1; end >= 0; end--) {
            if (data[end] == PATH_SPLIT) break;
        }

        return base::string_from_range(data, data + end);
    }

}  // namespace spargel::util
