#include <spargel/base/string.h>
#include <string.h>

namespace spargel::base {

String::String() {}
String::~String() {}

CString::CString() {
  _data = nullptr;
  _length = 0;
  _alloc = Allocator::global();
}
CString::~CString() {
  if (_data)
    _alloc->deallocate((u8*)_data,
                       MemoryLayout::defaultArrayLayout<char>(_length + 1));
}

void CString::initWithLiteral(char const* str) {
  _length = (ssize)strlen(str);
  _data = _alloc->allocateTypedArray<char>(_length + 1);
  memcpy(_data, str, _length + 1);
}

void CString::initByCopy(CString const& that) { initWithLiteral(that.data()); }
void CString::initByMove(CString& that) {
  _data = that._data;
  _length = that._length;
  that._data = nullptr;
  that._length = 0;
}

char* CString::data() { return _data; }
char const* CString::data() const { return _data; }
ssize CString::length() const { return _length; }

char& CString::operator[](ssize i) { return _data[i]; }
char const& CString::operator[](ssize i) const { return _data[i]; }

bool operator==(CString const& lhs, CString const& rhs) {
  if (lhs._data == nullptr) return rhs._data == nullptr;
  if (rhs._data == nullptr) return lhs._data == nullptr;
  return strcmp(lhs._data, rhs._data) == 0;
}

bool operator!=(CString const& lhs, CString const& rhs) {
  return !(lhs == rhs);
}

}  // namespace spargel::base
