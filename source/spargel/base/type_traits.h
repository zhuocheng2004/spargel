#pragma once

namespace spargel::base {

    template <typename T>
    using add_const = T const;

    template <typename T>
    using add_lvalue_reference = __add_lvalue_reference(T);

    template <typename T>
    using add_rvalue_reference = __add_rvalue_reference(T);

    template <typename T>
    using remove_reference = __remove_reference_t(T);

    template <typename T>
    using remove_cvref = __remove_cvref(T);

    template <typename T>
    using decay = __decay(T);

    template <typename T>
    add_rvalue_reference<T> declval()
    {
        static_assert(false, "declval not allowed in an evaluated context");
    }

    template <typename L, typename R>
    concept is_assignable = __is_assignable(L, R);

    template <typename T>
    concept is_copy_assignable =
        is_assignable<add_lvalue_reference<T>, add_lvalue_reference<add_const<T>>>;

    template <typename T>
    concept is_move_assignable =
        is_assignable<add_lvalue_reference<T>, add_rvalue_reference<T>>;

    template <typename B, typename D>
    concept is_base = __is_base_of(B, D);

    template <typename T, typename... Args>
    concept is_constructible = __is_constructible(T, Args...);

    template <typename T>
    concept is_copy_constructible =
        is_constructible<T, add_lvalue_reference<add_const<T>>>;

    template <typename T>
    concept is_move_constructible =
        is_constructible<T, add_rvalue_reference<T>>;

    template <typename From, typename To>
    concept is_conertible = __is_convertible(From, To);

    template <typename T>
    concept is_destructible = __is_destructible(T);

    template <typename T>
    concept is_trivially_destructible = __is_trivially_destructible(T);

    template <typename T>
    concept is_trivially_relocatable = __is_trivially_relocatable(T);

    template <typename L, typename R>
    concept is_same = __is_same(L, R);

    template <typename T>
    constexpr remove_reference<T>&& move(T&& t)
    {
        return static_cast<remove_reference<T>&&>(t);
    }

    template <typename T>
    constexpr T&& forward(remove_reference<T>& t)
    {
        return static_cast<T&&>(t);
    }

}
