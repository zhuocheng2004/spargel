#pragma once

#include <spargel/base/meta.h>
#include <spargel/base/tag_invoke.h>

namespace spargel::base {

    namespace __swap {
        struct swap {
            template <typename T>
            constexpr void operator()(T& lhs, T& rhs) const {
                tag_invoke(swap{}, lhs, rhs);
            }
        };
        template <typename T>
        void tag_invoke(swap, T& lhs, T& rhs) {
            T tmp(move(rhs));
            rhs = move(lhs);
            lhs = move(tmp);
        }
    }  // namespace __swap

    inline constexpr __swap::swap swap{};

}  // namespace spargel::base
