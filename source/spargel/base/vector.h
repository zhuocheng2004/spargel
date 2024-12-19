#pragma once

#include <spargel/base/algorithm.h>
#include <spargel/base/allocator.h>
#include <spargel/base/assert.h>
#include <spargel/base/meta.h>
#include <spargel/base/object.h>
#include <spargel/base/tag_invoke.h>
#include <spargel/base/types.h>

namespace spargel::base {

    namespace __vector {

        template <typename T>
        class vector {
        public:
            vector() = default;

            vector(vector const& other) {
                allocate(other.count());
                copy_from(other.begin(), other.end());
            }
            vector& operator=(vector const& other) {
                vector tmp(other);
                swap(*this, tmp);
                return *this;
            }

            vector(vector&& other) { swap(*this, other); }
            vector& operator=(vector&& other) {
                vector tmp(move(other));
                swap(*this, tmp);
                return *this;
            }

            ~vector() {
                if (_begin != nullptr) {
                    destruct_items();
                    deallocate();
                }
            }

            bool empty() const { return _end == _begin; }

            // construct and push back
            template <typename... Args>
            void push(Args&&... args) {
                if (_end >= _capacity) {
                    grow(capacity() + 1);
                }
                construct_at(_end, forward<Args>(args)...);
                _end++;
            }

            void pop() {
                spargel_assert(_begin < _end);
                _end--;
            }

            void clear() {
                destruct_items();
                _end = _begin;
            }

            void reserve(usize capacity) {
                if (_begin + capacity > _capacity) {
                    grow(capacity);
                }
            }

            // unsafe;
            void set_count(usize count) { _end = _begin + count; }

            T& operator[](usize i) {
                spargel_assert(_begin + i < _end);
                return _begin[i];
            }
            T const& operator[](usize i) const {
                spargel_assert(_begin + i < _end);
                return _begin[i];
            }

            T* data() { return _begin; }
            T const* data() const { return _begin; }
            usize count() const { return _end - _begin; }
            usize capacity() const { return _capacity - _begin; }

            T* begin() { return _begin; }
            T const* begin() const { return _begin; }
            T* end() { return _end; }
            T const* end() const { return _end; }

            friend void tag_invoke(tag<swap>, vector& lhs, vector& rhs) {
                swap(lhs._begin, rhs._begin);
                swap(lhs._end, rhs._end);
                swap(lhs._capacity, rhs._capacity);
            }

        private:
            usize next_capacity(usize count) {
                auto new_capacity = capacity() * 2;
                if (new_capacity < count) {
                    new_capacity = count;
                }
                return new_capacity;
            }

            void grow(usize need) {
                usize old_count = count();
                auto new_capacity = next_capacity(need);
                T* new_begin = static_cast<T*>(_alloc->alloc(sizeof(T) * new_capacity));
                if (_begin != nullptr) {
                    move_items(new_begin);
                    destruct_items();
                    deallocate();
                }
                _begin = new_begin;
                _end = _begin + old_count;
                _capacity = _begin + new_capacity;
            }

            void move_items(T* ptr) {
                for (T* iter = _begin; iter < _end; iter++, ptr++) {
                    construct_at(ptr, move(*iter));
                }
            }

            void destruct_items() {
                for (T* iter = _begin; iter < _end; iter++) {
                    destruct_at(iter);
                }
            }

            void allocate(usize capacity) {
                _begin = static_cast<T*>(_alloc->alloc(sizeof(T) * capacity));
                _end = _begin;
                _capacity = _begin + capacity;
            }

            void deallocate() { _alloc->free(_begin, sizeof(T) * capacity()); }

            void copy_from(T const* begin, T const* end) {
                for (; begin < end; begin++, _end++) {
                    construct_at(_end, *begin);
                }
            }

            T* _begin = nullptr;
            T* _end = nullptr;
            T* _capacity = nullptr;
            allocator* _alloc = default_allocator();
        };

    }  // namespace __vector

    using __vector::vector;

}  // namespace spargel::base
