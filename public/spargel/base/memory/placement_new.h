#pragma once

#include <spargel/base/types.h>

namespace spargel::base {

struct placement_new_tag {};

}  // namespace spargel::base

void* operator new(size_t size, void* ptr,
                   spargel::base::placement_new_tag) noexcept;
