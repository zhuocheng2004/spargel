#include <spargel/base/assert.h>
#include <spargel/util/path.h>

#if SPARGEL_IS_WINDOWS

int main() {
    auto p = spargel::util::dirname(spargel::base::string("C:\\hello\\world"));
    spargel_assert(p.length() == 8);
    auto q = spargel::util::dirname(spargel::base::string("D:\\hello\\world\\"));
    spargel_assert(q.length() == 14);
    return 0;
}

#else  // SPARGEL_IS_WINDOWS

int main() {
    auto p = spargel::util::dirname(spargel::base::string("/hello/world"));
    spargel_assert(p.length() == 6);
    auto q = spargel::util::dirname(spargel::base::string("/hello/world/"));
    spargel_assert(q.length() == 12);
    return 0;
}

#endif
