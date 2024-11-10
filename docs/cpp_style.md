# Spargel C++ Style Guide

## Rule of Zero

Classes that have user-defined (including `= default`) destructors, copy/move
constructors or copy/move assignment operators should deal exclusively with
ownership.
Other classes should not have custom destructors, copy/move constructors or
copy/move assignment operators.

Decision: If you can avoid, do.

## Rule of Three

> If a class requires a user-defined destructor, a user-defined copy constructor,
> or a user-defined copy-assignment operator, it almost certainly requires
> all three.

Decision:
See Rule of Five.

## Rule of Five

> The presence of a user-defined destructor, copy-constructor, or copy-assignment
> operator prevents implicit definition of the move-constructor and the move-
> assignment operator.

Decision:
Either zero of five.

## Polymorphic Classes should Suppress Public Copy or Move

See Core Guidelines C.67.

Decision:
Declare copy or move functions of a polymorphic class as deleted.

## Do not Use RTTI

Reason: RTTI increases binary size considerably.

Decision:
Use a handrolled version.
See [LLVM RTTI](https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html).

## Do not Use Exceptions

Reason: Exceptions need RTTI.

## Pre-C++11 Banned Features

### `<vector>`

## C++11 Banned Features

### Inline Namespaces

```cpp
inline namespace foo { /* ... */ }
```

### `wchar_t`

### `<cctype>`, `<wctype>`

Reason:
These have dependence on the C locale.

### `<exception>`

Reason:
Exceptions are not allowed.

### `<regex>`

Reason:
Poor support.

### `<thread>`

## C++17 Banned Features

### UTF-8 Literals

### `std::execution`

### `std::any`

### `std::filesystem`

### `std::variant`

## C++20 Banned Features

### Modules
