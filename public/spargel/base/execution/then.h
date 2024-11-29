#pragma once

namespace spargel::base {

namespace impl {

struct then_t {
    template <typename S, typename F>
    auto operator()(S&& sender, F func) const
    {
    }
};

}  // namespace impl

inline constexpr impl::then_t then{};

}  // namespace spargel::base
