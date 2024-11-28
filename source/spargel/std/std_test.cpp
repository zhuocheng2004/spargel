#include <assert.h>
#include <spargel/base/base.h>
#include <spargel/std/algorithm/max.h>
#include <spargel/std/container/vector.h>
#include <spargel/std/execution/sender.h>
#include <spargel/std/memory/allocator.h>
#include <spargel/std/memory/construct.h>
#include <spargel/std/memory/placement_new.h>
#include <spargel/std/meta/add_const.h>
#include <spargel/std/meta/add_reference.h>
#include <spargel/std/meta/forward.h>
#include <spargel/std/meta/is_assignable.h>
#include <spargel/std/meta/is_base.h>
#include <spargel/std/meta/is_constructible.h>
#include <spargel/std/meta/is_convertible.h>
#include <spargel/std/meta/is_destructible.h>
#include <spargel/std/meta/is_same.h>
#include <spargel/std/meta/move.h>
#include <spargel/std/meta/remove_reference.h>
#include <stdio.h>

static_assert(std::is_same<int, int>);
static_assert(!std::is_same<int, int&>);

static_assert(std::is_same<int const, std::add_const<int>>);

static_assert(std::is_same<int&, std::add_lvalue_reference<int>>);
static_assert(std::is_same<int&, std::add_lvalue_reference<int&>>);
static_assert(std::is_same<int&, std::add_lvalue_reference<int&&>>);

static_assert(std::is_same<int&&, std::add_rvalue_reference<int>>);
static_assert(std::is_same<int&, std::add_rvalue_reference<int&>>);
static_assert(std::is_same<int&&, std::add_rvalue_reference<int&&>>);

static_assert(!std::is_assignable<int, int>);
static_assert(!std::is_assignable<int, int&>);
static_assert(!std::is_assignable<int, int&&>);
static_assert(std::is_assignable<int&, int>);
static_assert(std::is_assignable<int&, int&>);
static_assert(std::is_assignable<int&, int&&>);
static_assert(!std::is_assignable<int&&, int>);
static_assert(!std::is_assignable<int&&, int&>);
static_assert(!std::is_assignable<int&&, int&&>);

static_assert(!std::is_base<int, int>);

static_assert(std::is_constructible<int, int>);
static_assert(std::is_constructible<int, double>);

static_assert(std::is_destructible<int>);

static_assert(std::is_same<int, std::remove_reference<int>>);
static_assert(std::is_same<int, std::remove_reference<int&>>);
static_assert(std::is_same<int, std::remove_reference<int&&>>);

namespace std {
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
// this is an awaitable
struct suspend_never {
    // first call this
    bool await_ready() const noexcept
    {
        printf("suspend_never::await_ready()\n");
        return true;
    }
    // this will not be called
    void await_suspend(coroutine_handle<>) const noexcept
    {
        printf("suspend_never::await_suspend()\n");
    }
    // last called
    void await_resume() const noexcept
    {
        printf("suspend_never::await_resume()\n");
    }
};
template <typename R, typename... Args>
struct coroutine_traits {
    using promise_type = R::promise_type;
};
}  // namespace std

struct basic_coro {
    struct promise {
        basic_coro get_return_object()
        {
            printf("basic_coro::get_return_object()\n");
            return basic_coro{};
        }
        void unhandled_exception()
        {
            printf("basic_coro::unhandled_exception()\n");
        }
        std::suspend_never initial_suspend()
        {
            printf("basic_coro::initial_suspend()\n");
            return {};
        }
        void return_void()
        {
            printf("basic_coro::return_void()\n");
        }
        std::suspend_never final_suspend() noexcept
        {
            printf("basic_coro::final_suspend()\n");
            return {};
        }
    };
    using promise_type = promise;
};

basic_coro coro2()
{
    co_return;
}

struct awaiter {
    bool await_ready()
    {
        printf("awaiter::await_ready()\n");
        return false;
    }
    void await_suspend(std::coroutine_handle<>)
    {
        printf("awaiter::await_suspend()\n");
        sbase_print_backtrace();
    }
    void await_resume() {}
};

basic_coro coro()
{
    printf("coro\n");
    sbase_print_backtrace();
    co_await awaiter();
    printf("before return\n");
    co_return;
}

int main()
{
    std::vector<int> vec;
    assert(vec.count() == 0);
    vec.push_back(1);
    assert(vec.count() == 1);
    assert(vec[0] == 1);
    // printf("test\n");
    // vec.pop_back();
    // printf("before coro\n");
    // sbase_print_backtrace();
    // coro();
    // printf("after coro\n");
    // sbase_print_backtrace();

    std::vector<int> vec2;
    std::swap(vec, vec2);
    assert(vec.count() == 0);
    assert(vec2.count() == 1);
    return 0;
}
