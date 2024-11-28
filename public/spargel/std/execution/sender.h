#pragma once

#include <spargel/std/meta/is_constructible.h>
#include <spargel/std/meta/is_destructible.h>
#include <spargel/std/meta/remove_cvref.h>

namespace std {

template <typename T>
concept queryable = is_destructible<T>;

// template<class Sndr>
// concept sender =
//   bool(enable-sender<remove_cvref_t<Sndr>>) &&
//   requires (const remove_cvref_t<Sndr>& sndr) {
//     { get_env(sndr) } -> queryable;
//   } &&
//   move_constructible<remove_cvref_t<Sndr>> &&
//   constructible_from<remove_cvref_t<Sndr>, Sndr>;

template <typename S>
concept sender =
    is_move_constructible<remove_cvref<S>> &&
    is_constructible<remove_cvref<S>, S> && requires(remove_cvref<S> const& s) {
        {
            get_env(s)
        } -> queryable;
    };

}  // namespace spargel::base
