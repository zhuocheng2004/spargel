# C++

C++ is a very complicated language.
Be very careful!

Important decisions in Spargel:

-   Do not use RTTI or exceptions.
-   Avoid the standard library.
-   Use `#pragma once`.

## Coroutine

A coroutine is a function whose definition contains any of `co_await`, `co_yield`, or `co_return`.

The `co_await expr` expression:

-   first obtain the awaiter object from `expr`
-   call `awaiter.await_ready()`
    -   if the result is false, then "the result is not ready, cannot be completed synchronously"
    -   this coroutine is suspended
    -   call `awaiter.await_suspend(handle)`, where `handle` is the handle to this coroutine
    -   if `await_suspend` returns `void`, return control to the caller, and this coroutine is suspended
    -   if `await_suspend` returns `true`, return control to the caller
    -   if `await_suspend` returns `false`, resume this coroutine
-   call `awaiter.await_resume()`, and its result is the result of `co_await expr`

`co_yield expr` is equivalent to `co_await promise.yield_value(expr)`.

The promise type is determined by compiler from the return type of the coroutine using `std::coroutine_tratis`.
For example, the promise type of `task<int> foo(int x)` is `std::coroutine_traits<task<int>, int>::promise_type`.

When a coroutine begins execution:

-   allocate the coroutine state object using `new`
-   copies function parameters to the coroutine state
-   call the constructor of the promise type
-   call `promise.get_return_object()`, and store it in a local variable, which will be returned to the caller on first suspension
-   call `promise.initial_suspend()`, and `co_await` its result
-   when `co_await promise.initial_suspend()` returns, start executing the body
-   when a suspension point is reached, the return object is returned to the caller/resumer
-   when `co_return` is reached, call `promise.return_void()` or `promise.return_value(expr)`
-   destroy all variables
-   `co_await promise.final_suspend()`
