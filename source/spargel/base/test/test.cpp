module;

#include <stdio.h>

#include <spargel/base/types.h>

module spargel.base.test;

namespace spargel::base {

test_handle make_and_register_test(char const* suite_name,
                                   char const* test_name, test_base* test)
{
    test_manager::instance().register_test(suite_name, test_name, test);
    return {};
}

test_manager& test_manager::instance()
{
    static test_manager manager;
    return manager;
}

void test_manager::register_test(char const* suite_name, char const* test_name,
                                 test_base* test)
{
    _tests.push_back(suite_name, test_name, test);
}

int test_manager::run()
{
    ssize count = _tests.count();
    for (ssize i = 0; i < count; i++) {
        _tests[i].test->test_body();
        printf("[%td/%td] test %s/%s\n", i + 1, count, _tests[i].suite_name,
               _tests[i].test_name);
    }
    return 0;
}

}  // namespace spargel::base
