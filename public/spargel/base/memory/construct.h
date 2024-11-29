#pragma once

#include <spargel/base/memory/placement_new.h>
#include <spargel/base/meta/forward.h>

namespace spargel::base {

template <typename T, typename... Args>
constexpr T* construct_at(void* ptr, Args&&... args)
{
    return new (ptr, placement_new_tag{}) T(forward<Args>(args)...);
}

}  // namespace spargel::base
