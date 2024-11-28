#pragma once

#include <spargel/base/types.h>

namespace spargel::base {

class default_allocator {
public:
    void* allocate(ssize size);
    void deallocate(void* ptr, ssize size);
};

}  // namespace spargel::base
