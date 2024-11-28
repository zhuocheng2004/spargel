#pragma once

#include <spargel/std/meta/move.h>

namespace std {

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
    T tmp = std::move(lhs);
    lhs = std::move(rhs);
    rhs = std::move(tmp);
}

}  // namespace __swap

inline constexpr __swap::swap_t swap{};

}  // namespace std
