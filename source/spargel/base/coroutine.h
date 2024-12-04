#pragma once

namespace spargel::base {

template <typename R, typename... Args>
struct coroutine_traits {
    using promise_type = R::promise_type;
};

template <typename P = void>
struct coroutine_handle;

template <>
struct coroutine_handle<void> {
    static constexpr coroutine_handle from_address(void* __addr) noexcept
    {
        coroutine_handle __tmp;
        __tmp.__handle_ = __addr;
        return __tmp;
    }
    void* __handle_ = nullptr;
};

template <typename P>
struct coroutine_handle {
    static constexpr coroutine_handle from_address(void* __addr) noexcept
    {
        coroutine_handle __tmp;
        __tmp.__handle_ = __addr;
        return __tmp;
    }
    constexpr void* address() const noexcept
    {
        return __handle_;
    }
    constexpr operator coroutine_handle<>() const noexcept
    {
        return coroutine_handle<>::from_address(address());
    }
    void* __handle_ = nullptr;
};

struct suspend_never {
    bool await_ready() const noexcept
    {
        return true;
    }
    void await_suspend(coroutine_handle<>) const noexcept {}
    void await_resume() const noexcept {}
};

}  // namespace spargel::base

namespace std {
using spargel::base::coroutine_handle;
using spargel::base::coroutine_traits;
using spargel::base::suspend_never;
}  // namespace std
