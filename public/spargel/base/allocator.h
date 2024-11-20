#pragma once

#include <spargel/base/types.h>

namespace spargel::base {

struct MemoryLayout {
  ssize size;
  ssize align;

  /// @brief the default layout of a type
  ///
  template <typename T>
  static constexpr MemoryLayout defaultLayout() {
    return MemoryLayout{sizeof(T), alignof(T)};
  }
  /// @brief the default layout of an array
  ///
  template <typename T>
  static constexpr MemoryLayout defaultArrayLayout(ssize length) {
    return MemoryLayout{(ssize)sizeof(T) * length, alignof(T)};
  }
};

/// @brief the allocator interface
///
/// All allocations in Spargel Engine go through this interface.
///
class Allocator {
 public:
  virtual ~Allocator();

  static Allocator* global();

  /// @brief allocate memory
  /// @return pointer to the allocated memory
  /// @retval nullptr if failed
  ///
  virtual u8* allocate(MemoryLayout layout) = 0;

  /// @brief deallocate memory
  /// @param ptr pointer to the allocated memory
  /// @param layout the layout of the allocated memory
  /// @remarks ptr must not be null
  ///
  virtual void deallocate(u8* ptr, MemoryLayout layout) = 0;

  /// @brief increase the allocated memory
  /// @param ptr pointer to the allocated memory
  /// @param old_layout layout of the memory that ptr points to
  /// @param new_layout layout of the new memory, which should have the same
  /// align as old_layout
  /// @retval nullptr if failed
  /// @remarks ptr must not be null
  ///
  virtual u8* grow(u8* ptr, MemoryLayout old_layout,
                   MemoryLayout new_layout) = 0;
};

}  // namespace spargel::base
