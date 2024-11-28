#pragma once

#include <spargel/std/algorithm/max.h>
#include <spargel/std/algorithm/swap.h>
#include <spargel/std/memory/allocator.h>
#include <spargel/std/memory/construct.h>
#include <spargel/std/memory/destroy.h>
#include <spargel/std/meta/forward.h>
#include <spargel/std/meta/is_destructible.h>
#include <spargel/std/meta/is_relocatable.h>
#include <spargel/std/meta/move.h>
#include <spargel/std/meta/tag_invoke.h>
#include <string.h>

namespace std {

namespace __vector {

// todo: max size
template <typename T>
class vector {
public:
    vector() = default;

    vector(vector const& other)
    {
        init_with_copy(other._begin, other._end, other.count());
    }
    vector(vector&& other)
    {
        swap(*this, other);
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

    ~vector()
    {
        if (_begin != nullptr) {
            for (; _begin < _end; _end--) {
                destroy_at(_end - 1);
            }
        }
    }

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
    void push_back(T&& value)
    {
        emplace_back(std::move(value));
    }

    template <typename... Args>
    void emplace_back(Args&&... args)
    {
        if (_end >= _capacity) {
            grow_one();
        }
        construct_at<T>(_end, std::forward<Args>(args)...);
        _end++;
    }

    void pop_back()
    {
        auto ptr = _end - 1;
        ptr->~T();
        _end--;
    }

    friend void tag_invoke(tag_t<swap>, vector& lhs, vector& rhs)
    {
        swap(lhs._begin, rhs._begin);
        swap(lhs._end, rhs._end);
        swap(lhs._capacity, rhs._capacity);
    }

private:
    // expects: request > 0
    ssize next_capacity(ssize request)
    {
        auto const cap = capacity();
        return max<ssize>(2 * cap, request);
    }

    void grow_one()
    {
        ssize new_cap = next_capacity(count() + 1);
        T* new_begin =
            static_cast<T*>(default_allocator{}.allocate(sizeof(T) * new_cap));
        if constexpr (is_trivially_relocatable<T>) {
            memcpy(new_begin, _begin, count() * sizeof(T));
        } else {
            for (T* iter = new_begin; _begin < _end; _begin++, new_begin++) {
                construct_at<T>(iter, *_begin);
            }
        }
        _capacity = new_begin + new_cap;
        _end = new_begin + count();
        _begin = new_begin;
    }

    void init_with_copy(T* begin, T* end, ssize count)
    {
        allocate(count);
        copy_to_end(begin, end);
    }

    void allocate(ssize count)
    {
        _begin = default_allocator{}.allocate(sizeof(T) * count);
        _end = _begin;
        _capacity = _begin + count;
    }

    void copy_to_end(T* begin, T* end)
    {
        for (; begin != end; begin++, _end++) {
            construct_at<T>(_end, *begin);
        }
    }

    T* _begin = nullptr;
    T* _end = nullptr;
    T* _capacity = nullptr;
};

}  // namespace __vector

using __vector::vector;

}  // namespace std
