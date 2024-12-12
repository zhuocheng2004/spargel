#include <spargel/base/assert.h>
#include <spargel/base/meta.h>
#include <spargel/base/vector.h>

using spargel::base::vector;

int main() {
    vector<int> v1;
    spargel_assert(v1.count() == 0);
    spargel_assert(v1.capacity() == 0);
    for (int i = 1; i <= 100; i++) {
        v1.push(i);
        spargel_assert(v1.count() == i);
    }
    spargel_assert(v1[50] == 51);
    vector<int> v2(spargel::base::move(v1));
    spargel_assert(v2.count() == 100);
    spargel_assert(v1.count() == 0);
    spargel_assert(v1.capacity() == 0);
    vector<int> v3 = v2;
    spargel_assert(v2.count() == 100);
    spargel_assert(v3.count() == 100);
    spargel_assert(v2[2] == 3);
    spargel_assert(v3[0] == 1);
    return 0;
}
