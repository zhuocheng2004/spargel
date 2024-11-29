#pragma once

#include <spargel/base/meta/decay.h>
#include <spargel/base/meta/forward.h>

namespace spargel::base {

namespace impl {

void tag_invoke();

struct tag_invoke_t {
    template <typename Tag, typename... Args>
    constexpr auto operator()(Tag tag, Args&&... args) const
    {
        return tag_invoke(forward<Tag>(tag), forward<Args>(args)...);
    }
};

}  // namespace impl

inline constexpr impl::tag_invoke_t tag_invoke{};

template <auto& t>
using tag_t = decay<decltype(t)>;

}  // namespace spargel::base
