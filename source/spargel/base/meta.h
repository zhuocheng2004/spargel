#pragma once

#include <spargel/base/compiler.h>

// notes:
//
// <<Itanium C++ ABI>>
//
// A type is considered non-trivial for the purpose of calls if:
// - it has a non-trivial copy constructor, move constructor, or destructor, or
// - all of its copy and move constructors are deleted.
//
// If a parameter type is a class type that is non-trivial for the purposes of calls, the caller
// must allocate space for a temporary and pass that temporary by reference.
//
// <<C++ Standard>>
//
// Scalar types, trivially copyable class types ([class.prop]), arrays of such types, and
// cv-qualified versions of these types are collectively called trivially copyable types.
//
// A trivially copyable class is a class:
// - that has at least one eligible copy constructor, move constructor, copy assignment operator, or
// move assignment operator ([special], [class.copy.ctor], [class.copy.assign]),
// - where each eligible copy constructor, move constructor, copy assignment operator, and move
// assignment operator is trivial, and
// - that has a trivial, non-deleted destructor ([class.dtor]).
//
// A non-template constructor for class X is a copy constructor if its first parameter is of type
// X&, const X&, volatile X& or const volatile X&, and either there are no other parameters or else
// all other parameters have default arguments ([dcl.fct.default]).
//
// A non-template constructor for class X is a move constructor if its first parameter is of type
// X&&, const X&&, volatile X&&, or const volatile X&&, and either there are no other parameters or
// else all other parameters have default arguments ([dcl.fct.default]).
//
// A copy/move constructor for class X is trivial if it is not user-provided and if
// - class X has no virtual functions ([class.virtual]) and no virtual base classes ([class.mi]),
// and
// - the constructor selected to copy/move each direct base class subobject is trivial, and
// - for each non-static data member of X that is of class type (or array thereof), the constructor
// selected to copy/move that member is trivial.
//
// note: user-provided means implicitly-defined or defaulted.
//

namespace spargel::base {

#if spargel_has_builtin(__is_same)
    template <typename S, typename T>
    inline constexpr bool is_same = __is_same(S, T);
#else
    namespace __is_same {
        template <typename S, typename T>
        struct is_same {
            static constexpr bool value = false;
        };
        template <typename T>
        struct is_same<T, T> {
            static constexpr bool value = false;
        };
    }  // namespace __is_same
    template <typename S, typename T>
    inline constexpr bool is_same = __is_same ::is_same<S, T>::value;
#endif

#if spargel_has_builtin(__remove_reference_t)
    template <typename T>
    using remove_reference = __remove_reference_t(T);
#elif spargel_has_builtin(__remove_reference)
    template <typename T>
    using remove_reference = __remove_reference(T);
#else
    namespace __remove_reference {
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
    }  // namespace __remove_reference
    template <typename T>
    using remove_reference = __remove_reference::remove_reference<T>::type;
#endif

    template <typename T>
    constexpr remove_reference<T>&& move(T&& t) {
        return static_cast<remove_reference<T>&&>(t);
    }

    template <typename T>
    constexpr T&& forward(remove_reference<T>& t) {
        return static_cast<T&&>(t);
    }

#if spargel_has_builtin(__decay)
    template <typename T>
    using decay = __decay(T);
#else
#error unimplemented
#endif

#if spargel_has_builtin(__is_convertible)
    template <typename S, typename T>
    inline constexpr bool is_convertible = __is_convertible(S, T);
#else
#error unimplemented
#endif

}  // namespace spargel::base
