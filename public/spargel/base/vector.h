#pragma once

#include <spargel/base/allocator.h>
#include <spargel/base/construct.h>
#include <spargel/base/types.h>

namespace spargel::base {

template <typename T>
class Vector {
 public:
  Vector() {
    _data = nullptr;
    _count = 0;
    _capacity = 0;
    _alloc = Allocator::global();
  }
  ~Vector() {
    if (_data)
      _alloc->deallocate((u8*)_data,
                         MemoryLayout::defaultArrayLayout<T>(_capacity));
  }

  /// @brief initialize the vector by copy
  void initByCopy(Vector const& that) {
    _alloc = that._alloc;
    _capacity = that._capacity;
    _count = that._count;
    _data = _alloc->allocateTypedArray<T>(_capacity);
    memcpy(_data, that._data, sizeof(T) * _capacity);
  }

  /// @brief initialize the vector by move
  void initByMove(Vector& that) {
    *this = that;
    that = Vector();
  }

  ssize count() const { return _count; }
  ssize capacity() const { return _capacity; }
  T* data() { return _data; }

  T& operator[](ssize i) { return _data[i]; }

  /// @brief append an element to the vector by copy
  void pushCopy(T const& value) {
    if (_count == _capacity) {
      growToAtLeast(_count + 1);
    }
    copyConstruct(_data[_count], value);
    _count++;
  }

  /// @brief append an element to the vector by move
  void pushMove(T& value) {
    if (_count == _capacity) {
      growToAtLeast(_count + 1);
    }
    moveConstruct(_data[_count], value);
    _count++;
  }

  /// @brief pop the last element
  /// @warning this does not call the destructor
  void pop() { _count--; }

  /// @brief set the count of the vector
  /// @warning unsafe
  void setCount(ssize count) { _count = count; }

 private:
  static constexpr ssize MINIMAL_CAPACITY = 8;

  //. @brief grow such that capacity >= count
  void growToAtLeast(ssize count) {
    if (_data) {
      // todo: overflow
      ssize cap2 = _capacity * 2;
      // this ensures exponential growth: new_cap >= 2 * capacity
      ssize new_cap = cap2 < count ? count : cap2;
      _data = (T*)_alloc->grow((u8*)_data,
                               MemoryLayout::defaultArrayLayout<T>(_capacity),
                               MemoryLayout::defaultArrayLayout<T>(new_cap));
      // todo: allocation failure
      _capacity = new_cap;
    } else {
      ssize new_cap = count < MINIMAL_CAPACITY ? MINIMAL_CAPACITY : count;
      _data = _alloc->allocateTypedArray<T>(new_cap);
      _capacity = new_cap;
    }
  }

  T* _data;
  ssize _count;
  ssize _capacity;
  Allocator* _alloc;
};

template <typename T>
struct DefaultCopyTraits<Vector<T>> {
  static void copyConstruct(Vector<T>& dst, Vector<T> const& src) {
    dst.initByCopy(src);
  }
};

template <typename T>
struct DefaultMoveTraits<Vector<T>> {
  static void moveConstruct(Vector<T>& dst, Vector<T>& src) {
    dst.initByMove(src);
  }
};

}  // namespace spargel::base
