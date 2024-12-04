#pragma once

#include <spargel/base/allocator.h>
#include <spargel/base/memory/construct.h>
#include <spargel/base/test/test_manager.h>

namespace spargel::base {

struct test_info {
    char const* suite_name;
    char const* test_name;
};

class test_base {
public:
    virtual void test_body() = 0;
};

struct test_handle {};

test_handle make_and_register_test(char const* suite_name,
                                   char const* test_name, test_base* test);

}  // namespace spargel::base

#define _TEST_CLASS_NAME(suite_name, test_name) \
    spargel_test_class_##suite_name##_##test_name##_test

#define TEST(suite_name, test_name)                                           \
    class _TEST_CLASS_NAME(suite_name, test_name) final                       \
        : public ::spargel::base::test_base {                                 \
    public:                                                                   \
        _TEST_CLASS_NAME(suite_name, test_name)() = default;                  \
                                                                              \
        static _TEST_CLASS_NAME(suite_name, test_name) & instance()           \
        {                                                                     \
            static _TEST_CLASS_NAME(suite_name, test_name) test;              \
            return test;                                                      \
        }                                                                     \
                                                                              \
        void test_body() override;                                            \
                                                                              \
    private:                                                                  \
        static ::spargel::base::test_handle const _handle;                    \
    };                                                                        \
    ::spargel::base::test_handle const _TEST_CLASS_NAME(suite_name,           \
                                                        test_name)::_handle = \
        ::spargel::base::make_and_register_test(                              \
            #suite_name, #test_name,                                          \
            &_TEST_CLASS_NAME(suite_name, test_name)::instance());            \
    void _TEST_CLASS_NAME(suite_name, test_name)::test_body()
