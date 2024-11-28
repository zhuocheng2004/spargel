#pragma once

#include <spargel/base/algorithm/max.h>
#include <spargel/base/algorithm/swap.h>
#include <spargel/base/memory/allocator.h>
#include <spargel/base/memory/construct.h>
#include <spargel/base/meta/forward.h>
#include <spargel/base/meta/tag_invoke.h>

namespace spargel::base {

namespace __vector {

// todo: max size
template <typename T>
class vector {
public:
    vector() = default;

    vector(vector const&) {}
    vector(vector&& other)
    {
        _begin = other._begin;
        _end = other._end;
        _capacity = other._capacity;
        other._begin = nullptr;
        other._end = nullptr;
        other._capacity = nullptr;
    }

    vector& operator=(vector const& other)
    {
        vector tmp(other);
        swap(*this, tmp);
        return *this;
    }

    vector& operator=(vector&& other)
    {
        vector tmp(move(other));
        swap(*this, tmp);
        return *this;
    }

    ~vector() {}

    T& operator[](ssize i)
    {
        return _begin[i];
    }
    T const& operator[](ssize i) const
    {
        return _begin[i];
    }

    T* data()
    {
        return _begin;
    }
    T const* data() const
    {
        return _begin;
    }

    ssize count() const
    {
        return _end - _begin;
    }

    ssize capacity() const
    {
        return _capacity - _begin;
    }

    void push_back(T const& value)
    {
        emplace_back(value);
    }

    template <typename... Args>
    void emplace_back(Args&&... args)
    {
        if (_end < _capacity) {
            construct_at<T>(_end, forward<Args>(args)...);
            _end++;
        } else {
        }
    }

    void pop_back() {}

    friend void tag_invoke(tag_t<swap>, vector& lhs, vector& rhs)
    {
        swap(lhs._begin, rhs._begin);
        swap(lhs._end, rhs._end);
        swap(lhs._capacity, rhs._capacity);
    }

private:
    ssize next_capacity(ssize request)
    {
        auto const cap = capacity();
        return max<ssize>(2 * cap, request);
    }

    T* _begin = nullptr;
    T* _end = nullptr;
    T* _capacity = nullptr;
};

}  // namespace __vector

using __vector::vector;

}  // namespace spargel::base
