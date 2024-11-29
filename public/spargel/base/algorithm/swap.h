#pragma once

#include <spargel/base/meta/move.h>

namespace spargel::base {

namespace __swap {

struct swap_t {
    template <typename T>
    void operator()(T& lhs, T& rhs) const
    {
        tag_invoke(swap_t{}, lhs, rhs);
    }
};

template <typename T>
void tag_invoke(swap_t, T& lhs, T& rhs)
{
    T tmp = move(lhs);
    lhs = move(rhs);
    rhs = move(tmp);
}

}  // namespace __swap

inline constexpr __swap::swap_t swap{};

}  // namespace spargel::base
