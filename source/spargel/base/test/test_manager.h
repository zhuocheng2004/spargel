#pragma once

#include <spargel/base/vector.h>

namespace spargel::base {

class test_base;

struct test_item {
    char const* suite_name;
    char const* test_name;
    test_base* test;
};

class test_manager {
public:
    static test_manager& instance();

    void register_test(char const* suite_name, char const* test_name,
                       test_base* test);

    int run();

private:
    vector<test_item> _tests;
};

}  // namespace spargel::base
