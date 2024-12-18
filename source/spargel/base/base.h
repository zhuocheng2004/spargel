#pragma once

#include <spargel/base/const.h>
#include <spargel/base/types.h>
#include <spargel/config.h>

namespace spargel::base {

    /* panic */

    [[noreturn]] void panic();

    [[noreturn]] void panic_at(char const* file, char const* func, u32 line);

#define spargel_panic_here() ::spargel::base::panic_at(__FILE__, __func__, __LINE__)

    /* backtrace */

    void print_backtrace();

    /* allocator */

    enum allocation_tag {
        ALLOCATION_BASE = 0,
        ALLOCATION_CODEC,
        ALLOCATION_ECS,
        ALLOCATION_GPU,
        ALLOCATION_UI,
        ALLOCATION_RESOURCE,

        _ALLOCATION_COUNT,
    };

    void* allocate(usize size, int tag);
    void* reallocate(void* ptr, usize old_size, usize new_size, int tag);
    void deallocate(void* ptr, usize size, int tag);

    void report_allocation();
    void check_leak();

}  // namespace spargel::base
