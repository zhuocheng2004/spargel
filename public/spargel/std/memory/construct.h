#pragma once

#include <spargel/std/memory/placement_new.h>
#include <spargel/std/meta/forward.h>

namespace std {

template <typename T, typename... Args>
constexpr T* construct_at(void* ptr, Args&&... args)
{
    return new (ptr) T(std::forward<Args>(args)...);
}

}  // namespace spargel::base
