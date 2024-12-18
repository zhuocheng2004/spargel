#include <spargel/base/assert.h>
#include <spargel/util/path.h>

int main() {
    auto p = spargel::util::dirname(spargel::base::string("/hello/world"));
    spargel_assert(p.length() == 6);
    auto q = spargel::util::dirname(spargel::base::string("/hello/world/"));
    spargel_assert(q.length() == 12);
    return 0;
}
