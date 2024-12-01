#include <assert.h>
#include <spargel/base/container/vector.h>
#include <spargel/base/test/test.h>

TEST(vector, basic_asserts)
{
    spargel::base::vector<int> vec;
    assert(vec.count() == 0);
    vec.push_back(1);
    assert(vec.count() == 1);
    assert(vec[0] == 1);

    vec.push_back(2);
    vec.push_back(3);
    vec.push_back(4);
    vec.push_back(5);

    assert(vec.count() == 5);
    assert(vec[4] == 5);

    spargel::base::vector<int> vec2;
    spargel::base::swap(vec, vec2);
    assert(vec.count() == 0);
    assert(vec2.count() == 5);

    spargel::base::vector<int> vec3(vec2);
    assert(vec2.count() == 5);
    assert(vec3.count() == 5);

    vec3.pop_back();
    assert(vec3.count() == 4);
}

TEST(vector, reserve) {
    spargel::base::vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    assert(vec.count() == 3);
    vec.reserve(100);
    assert(vec.capacity() >= 100);
}
