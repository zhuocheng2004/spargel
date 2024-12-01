#pragma once

#include <spargel/base/algorithm/max.h>
#include <spargel/base/algorithm/swap.h>
#include <spargel/base/assert/assert.h>
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

    vector(T const* begin, T const* end)
    {
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
            destroy(_begin, _end);
            deallocate();
        }
    }

    /// @brief access element by offset
    T& operator[](ssize i)
    {
        EXPECTS(i >= 0 && i < count());
        return _begin[i];
    }
    T const& operator[](ssize i) const
    {
        EXPECTS(i >= 0 && i < count());
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
            grow_to(count() + 1);
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

    void reserve(ssize n)
    {
        if (n > capacity()) {
            grow_to(n);
        }
    }

    /// unsafe
    /// this will not construct/destruct/allocate/deallocate
    void set_count(ssize cnt)
    {
        _end = _begin + cnt;
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

    void grow_to(ssize req)
    {
        auto const cnt = count();
        ssize new_cap = next_capacity(req);
        T* new_begin =
            static_cast<T*>(default_allocator{}.allocate(sizeof(T) * new_cap));
        if (_begin != nullptr) {
            relocate(new_begin, _begin, _end);
            destroy(_begin, _end);
            deallocate();
        }
        _begin = new_begin;
        _end = _begin + cnt;
        _capacity = _begin + new_cap;
    }

    void init_with_copy(T const* begin, T const* end)
    {
        if (begin != nullptr) {
            auto count = end - begin;
            allocate(count);
            relocate(_end, begin, end);
            _end = _begin + count;
        }
    }

    // 1. allocate
    // 2. deallocate
    // 3. relocate to end
    // 4. destroy

    void allocate(ssize count)
    {
        EXPECTS(count > 0);
        _begin =
            static_cast<T*>(default_allocator{}.allocate(sizeof(T) * count));
        _end = _begin;
        _capacity = _begin + count;
    }

    void deallocate()
    {
        default_allocator{}.deallocate(_begin, sizeof(T) * capacity());
        _begin = nullptr;
        _end = nullptr;
        _capacity = nullptr;
    }

    // relocate objects in [begin, end) to target
    void relocate(T* target, T* begin, T* end)
    {
        EXPECTS(begin < end);
        if constexpr (is_trivially_relocatable<T>) {
            ssize cnt = end - begin;
            memcpy(target, begin, sizeof(T) * cnt);
        } else {
            for (; begin != end; target++, begin++) {
                construct_at<T>(target, move(*begin));
            }
        }
    }
    void relocate(T* target, T const* begin, T const* end)
    {
        EXPECTS(begin < end);
        if constexpr (is_trivially_relocatable<T>) {
            ssize cnt = end - begin;
            memcpy(target, begin, sizeof(T) * cnt);
        } else {
            for (; begin != end; target++, begin++) {
                construct_at<T>(target, *begin);
            }
        }
    }

    // destroy objects in [begin, end) from back to front
    void destroy(T* begin, T* end)
    {
        EXPECTS(begin < end);
        T* iter = end;
        while (iter != begin) {
            destroy_at(--iter);
        }
    }

    T* _begin = nullptr;
    T* _end = nullptr;
    T* _capacity = nullptr;
};

}  // namespace __vector

using __vector::vector;

}  // namespace spargel::base
