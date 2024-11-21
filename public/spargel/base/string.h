#pragma once

#include <spargel/base/allocator.h>
#include <spargel/base/types.h>

namespace spargel::base {

// string
class String final {
 public:
  String();
  ~String();
};

// c string, i.e. null terminated
class CString final {
 public:
  CString();
  ~CString();

  // CString owns memory, and thus cannot be copied trivially
  CString(CString const&) = delete;
  CString(CString&&) = delete;

  void initWithLiteral(char const* str);

  void initByCopy(CString const& that);
  void initByMove(CString& that);

  char* data();
  char const* data() const;
  ssize length() const;

  char& operator[](ssize i);
  char const& operator[](ssize i) const;

  friend bool operator==(CString const& lhs, CString const& rhs);
  friend bool operator!=(CString const& lhs, CString const& rhs);

 private:
  char* _data;
  ssize _length;
  Allocator* _alloc;
};

}  // namespace spargel::base
