#pragma once

#if defined(__has_builtin)
#define spargel_has_builtin(x) __has_builtin(x)
#else
#define spargel_has_builtin(x) 0
#endif

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
//
// note:
//
// There are three different kinds of operations to an object.
// - copy: create a copy of the object.
// - move: move the object to another object; both the two objects are alive after move
// - relocate: place the object into a new address; the original object becomes dead
//
// - trivially copyable: the type can be copied via memcpy
// - trivially relocatable:
