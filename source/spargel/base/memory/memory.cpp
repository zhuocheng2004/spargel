module;

#include <stddef.h>

module spargel.base.memory;

void* operator new(size_t size, void* ptr,
                   spargel::base::placement_new_tag) noexcept
{
    return ptr;
}
