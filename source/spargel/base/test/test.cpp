#include <spargel/base/test/test.h>

namespace spargel::base {

    test_handle make_and_register_test(char const* suite_name, char const* test_name,
                                       test_base* test) {
        test_manager::instance().register_test(suite_name, test_name, test);
        return {};
    }

}  // namespace spargel::base
