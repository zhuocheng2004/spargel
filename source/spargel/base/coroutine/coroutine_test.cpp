#include <spargel/base/coroutine/coroutine.h>

struct basic_coro {
    struct promise {
        basic_coro get_return_object()
        {
            return basic_coro{};
        }
        void unhandled_exception() {}
        std::suspend_never initial_suspend()
        {
            return {};
        }
        void return_void() {}
        std::suspend_never final_suspend() noexcept
        {
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
        return true;
    }
    void await_suspend(std::coroutine_handle<>) {}
    void await_resume() {}
};

basic_coro coro()
{
    co_await awaiter();
    co_return;
}

int main()
{
    coro();
    return 0;
}
