#include <spargel/base/memory/placement_new.h>

void* operator new(size_t size, void* ptr,
                   spargel::base::placement_new_tag) noexcept
{
    return ptr;
}
