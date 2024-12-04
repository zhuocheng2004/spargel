#include <spargel/base/vector.h>
#include <spargel/base/assert.h>
#include <spargel/base/test/test.h>

TEST(vector, basic_asserts) {
    spargel::base::vector<int> vec;
    ASSERT(vec.count() == 0);
    vec.push_back(1);
    ASSERT(vec.count() == 1);
    ASSERT(vec[0] == 1);

    vec.push_back(2);
    vec.push_back(3);
    vec.push_back(4);
    vec.push_back(5);

    ASSERT(vec.count() == 5);
    ASSERT(vec[4] == 5);

    spargel::base::vector<int> vec2;
    spargel::base::swap(vec, vec2);
    ASSERT(vec.count() == 0);
    ASSERT(vec2.count() == 5);

    spargel::base::vector<int> vec3(vec2);
    ASSERT(vec2.count() == 5);
    ASSERT(vec3.count() == 5);

    vec3.pop_back();
    ASSERT(vec3.count() == 4);
}

TEST(vector, reserve) {
    spargel::base::vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    ASSERT(vec.count() == 3);
    vec.reserve(100);
    ASSERT(vec.capacity() >= 100);
}
