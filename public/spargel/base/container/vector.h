#pragma once

#include <spargel/base/algorithm/max.h>
#include <spargel/base/algorithm/swap.h>
#include <spargel/base/memory/allocator.h>
#include <spargel/base/memory/construct.h>
#include <spargel/base/memory/destroy.h>
#include <spargel/base/meta/forward.h>
#include <spargel/base/meta/is_destructible.h>
#include <spargel/base/meta/is_relocatable.h>
#include <spargel/base/meta/move.h>
#include <spargel/base/meta/tag_invoke.h>
#include <string.h>

namespace spargel::base {

namespace __vector {

// todo: max size
template <typename T>
class vector {
public:
    /// @brief create empty vector
    vector() = default;

    vector(T const* begin, T const* end) {
        init_with_copy(begin, end);
    }

    /// @brief copy from another vector
    vector(vector const& other)
    {
        init_with_copy(other._begin, other._end);
    }

    /// @brief move from another vector
    vector(vector&& other)
    {
        swap(*this, other);
    }

    /// @brief copy assignment
    vector& operator=(vector const& other)
    {
        vector tmp(other);
        swap(*this, tmp);
        return *this;
    }

    /// @brief move assignment
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
            default_allocator{}.deallocate(_begin, sizeof(T) * capacity());
        }
    }

    /// @brief access element by offset
    T& operator[](ssize i)
    {
        return _begin[i];
    }
    T const& operator[](ssize i) const
    {
        return _begin[i];
    }

    /// @brief access underlying data
    T* data()
    {
        return _begin;
    }
    T const* data() const
    {
        return _begin;
    }

    /// @brief number of elements (slow)
    ssize count() const
    {
        return _end - _begin;
    }

    /// @brief capacity of vector
    ssize capacity() const
    {
        return _capacity - _begin;
    }

    /// @brief construct an element at end
    template <typename... Args>
    void push_back(Args&&... args)
    {
        if (_end >= _capacity) {
            grow_one();
        }
        construct_at<T>(_end, forward<Args>(args)...);
        _end++;
    }

    /// @brief remove one element from back
    void pop_back()
    {
        auto ptr = _end - 1;
        ptr->~T();
        _end--;
    }

private:
    friend void tag_invoke(tag_t<swap>, vector& lhs, vector& rhs)
    {
        swap(lhs._begin, rhs._begin);
        swap(lhs._end, rhs._end);
        swap(lhs._capacity, rhs._capacity);
    }

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
                construct_at<T>(iter, move(*_begin));
            }
        }
        if (_begin != nullptr) {
            for (T* iter = _end - 1; iter >= _begin; iter--) {
                destroy_at(iter);
            }
            default_allocator{}.deallocate(_begin, sizeof(T) * capacity());
        }
        _capacity = new_begin + new_cap;
        _end = new_begin + count();
        _begin = new_begin;
    }

    void init_with_copy(T const* begin, T const* end)
    {
        auto count = end - begin;
        allocate(count);
        copy_to_end(begin, end);
    }

    void allocate(ssize count)
    {
        _begin =
            static_cast<T*>(default_allocator{}.allocate(sizeof(T) * count));
        _end = _begin;
        _capacity = _begin + count;
    }

    void copy_to_end(T const* begin, T const* end)
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

}  // namespace spargel::base
