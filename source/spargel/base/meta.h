#pragma once

#include <spargel/base/compiler.h>
#include <spargel/base/types.h>

// notes:
//
// <<Itanium C++ ABI>>
//
// A type is considered non-trivial for the purpose of calls if:
// - it has a non-trivial copy constructor, move constructor, or destructor, or
// - all of its copy and move constructors are deleted.
//
// If a parameter type is a class type that is non-trivial for the purposes of
// calls, the caller must allocate space for a temporary and pass that temporary
// by reference.
//
// <<C++ Standard>>
//
// Scalar types, trivially copyable class types ([class.prop]), arrays of such
// types, and cv-qualified versions of these types are collectively called
// trivially copyable types.
//
// A trivially copyable class is a class:
// - that has at least one eligible copy constructor, move constructor, copy
// assignment operator, or move assignment operator ([special],
// [class.copy.ctor], [class.copy.assign]),
// - where each eligible copy constructor, move constructor, copy assignment
// operator, and move assignment operator is trivial, and
// - that has a trivial, non-deleted destructor ([class.dtor]).
//
// A non-template constructor for class X is a copy constructor if its first
// parameter is of type X&, const X&, volatile X& or const volatile X&, and
// either there are no other parameters or else all other parameters have
// default arguments ([dcl.fct.default]).
//
// A non-template constructor for class X is a move constructor if its first
// parameter is of type X&&, const X&&, volatile X&&, or const volatile X&&, and
// either there are no other parameters or else all other parameters have
// default arguments ([dcl.fct.default]).
//
// A copy/move constructor for class X is trivial if it is not user-provided and
// if
// - class X has no virtual functions ([class.virtual]) and no virtual base
// classes ([class.mi]), and
// - the constructor selected to copy/move each direct base class subobject is
// trivial, and
// - for each non-static data member of X that is of class type (or array
// thereof), the constructor selected to copy/move that member is trivial.
//
// note: user-provided means implicitly-defined or defaulted.
//

namespace spargel::base {

#if spargel_has_builtin(__is_same)
    template <typename S, typename T>
    inline constexpr bool is_same = __is_same(S, T);
#else
    namespace _is_same {
        template <typename S, typename T>
        struct is_same {
            static constexpr bool value = false;
        };
        template <typename T>
        struct is_same<T, T> {
            static constexpr bool value = false;
        };
    }  // namespace _is_same
    template <typename S, typename T>
    inline constexpr bool is_same = _is_same ::is_same<S, T>::value;
#endif

#if spargel_has_builtin(__remove_reference_t)
    template <typename T>
    using remove_reference = __remove_reference_t(T);
#elif spargel_has_builtin(__remove_reference)
    template <typename T>
    using remove_reference = __remove_reference(T);
#else
    namespace _remove_reference {
        template <typename T>
        struct remove_reference {
            using type = T;
        };
        template <typename T>
        struct remove_reference<T&> {
            using type = T;
        };
        template <typename T>
        struct remove_reference<T&&> {
            using type = T;
        };
    }  // namespace _remove_reference
    template <typename T>
    using remove_reference = _remove_reference::remove_reference<T>::type;
#endif

    template <typename T>
    constexpr remove_reference<T>&& move(T&& t) {
        return static_cast<remove_reference<T>&&>(t);
    }

    template <typename T>
    constexpr T&& forward(remove_reference<T>& t) {
        return static_cast<T&&>(t);
    }

#if spargel_has_builtin(__remove_cv)
    template <typename T>
    using remove_cv = __remove_cv(T);
#else
    namespace _remove_cv {
        template <typename T>
        struct remove_cv {
            using type = T;
        };
        template <typename T>
        struct remove_cv<const T> {
            using type = T;
        };
        template <typename T>
        struct remove_cv<volatile T> {
            using type = T;
        };
        template <typename T>
        struct remove_cv<const volatile T> {
            using type = T;
        };
    }  // namespace _remove_cv
    template <typename T>
    using remove_cv = _remove_cv::remove_cv<T>::type;
#endif

#if spargel_has_builtin(__is_array)
    template <typename T>
    inline constexpr bool is_array = __is_array(T);
#else
    namespace _is_array {
        template <typename T>
        struct is_array {
            static constexpr bool value = false;
        };
        template <typename T>
        struct is_array<T[]> {
            static constexpr bool value = true;
        };
        template <typename T, usize N>
        struct is_array<T[N]> {
            static constexpr bool value = true;
        };
    }  // namespace _is_array
    template <typename T>
    inline constexpr bool is_array = _is_array::is_array<T>::value;
#endif

#if spargel_has_builtin(__remove_extent)
    template <typename T>
    using remove_extent = __remove_extent(T);
#else
    namespace _remove_extent {
        template <typename T>
        struct remove_extent {
            using type = T;
        };
        template <typename T>
        struct remove_extent<T[]> {
            using type = T;
        };
        template <typename T, usize N>
        struct remove_extent<T[N]> {
            using type = T;
        };
    }  // namespace _remove_extent
    template <typename T>
    using remove_extent = _remove_extent::remove_extent<T>::type;
#endif

    template <typename T>
    struct type_identity {
        using type = T;
    };

#if spargel_has_builtin(__is_referenceable)
    template <typename T>
    inline constexpr bool is_referenceable = __is_referenceable(T);
#else
    template <typename T>
    inline constexpr bool is_referenceable = requires { is_same<T&, T&>; };
#endif

#if spargel_has_builtin(__is_void)
    template <typename T>
    inline constexpr bool is_void = __is_void(T);
#else
    template <typename T>
    inline constexpr bool is_void = is_same<void, remove_cv<T>>;
#endif

#if spargel_has_builtin(__add_pointer)
    template <typename T>
    using add_pointer = __add_pointer(T);
#else
    namespace _add_pointer {
        template <typename T, bool = is_referenceable<T> || is_void<T>>
        struct add_pointer {
            using type = remove_reference<T>;
        };
        template <typename T>
        struct add_pointer<T, false> {
            using type = T;
        };
    }  // namespace _add_pointer
    template <typename T>
    using add_pointer = _add_pointer::add_pointer<T>::type;
#endif

    namespace _conditional {
        template <bool b, typename S, typename T>
        struct conditional;
        template <typename S, typename T>
        struct conditional<true, S, T> {
            using type = S;
        };
        template <typename S, typename T>
        struct conditional<false, S, T> {
            using type = T;
        };
    }  // namespace _conditional
    template <bool b, typename S, typename T>
    using conditional = _conditional::conditional<b, S, T>;

#if spargel_has_builtin(__is_const)
#else
    namespace _is_const {
        template <typename T>
        struct is_const {
            static constexpr bool value = false;
        };
        template <typename T>
        struct is_const<const T> {
            static constexpr bool value = true;
        };
    }  // namespace _is_const
    template <typename T>
    inline constexpr bool is_const = _is_const::is_const<T>::value;
#endif

#if spargel_has_builtin(__is_reference)
#else
    namespace _is_reference {
        template <typename T>
        struct is_reference {
            static constexpr bool value = false;
        };
        template <typename T>
        struct is_reference<T&> {
            static constexpr bool value = true;
        };
        template <typename T>
        struct is_reference<T&&> {
            static constexpr bool value = true;
        };
    }  // namespace _is_reference
    template <typename T>
    inline constexpr bool is_reference = _is_reference::is_reference<T>::value;
#endif

#if spargel_has_builtin(__is_function)
    template <typename T>
    inline constexpr bool is_function = __is_function(T);
#else
    template <typename T>
    inline constexpr bool is_function = !is_const<const T> && !is_reference<T>;
#endif

#if spargel_has_builtin(__decay)
    template <typename T>
    using decay = __decay(T);
#else
    template <typename T>
    using decay = conditional<is_array<T>, add_pointer<remove_extent<T>>,
                              conditional<is_function<T>, add_pointer<T>, remove_cv<T>>>;
#endif

#if spargel_has_builtin(__add_rvalue_reference)
    template <typename T>
    using add_rvalue_reference = __add_rvalue_reference(T);
#else
    namespace _add_rvalue_reference {
        template <typename T, bool = is_referenceable<T>>
        struct add_rvalue_reference {
            using type = T&&;
        };
        template <typename T>
        struct add_rvalue_reference<T, false> {
            using type = T;
        };
    }  // namespace _add_rvalue_reference
    template <typename T>
    using add_rvalue_reference = _add_rvalue_reference::add_rvalue_reference<T>::type;
#endif

    template <typename T>
    consteval add_rvalue_reference<T> declval() {
        static_assert(false);
    }

#if spargel_has_builtin(__is_convertible)
    template <typename S, typename T>
    inline constexpr bool is_convertible = __is_convertible(S, T);
#else
    template <typename S, typename T>
    inline constexpr bool is_convertible = requires { declval<void (&)(T)>()(declval<S>()); };
#endif

}  // namespace spargel::base
