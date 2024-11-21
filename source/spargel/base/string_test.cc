#include <assert.h>
#include <spargel/base/string.h>

int main() {
  using spargel::base::CString;
  CString str1;
  CString str2;
  assert(str1.data() == nullptr);
  assert(str1.length() == 0);
  str1.initWithLiteral("hello, world!");
  assert(str1.length() == 13);
  str2.initByMove(str1);
  assert(str2.length() == 13);
  assert(str1.data() == nullptr);
  assert(str1.length() == 0);
  return 0;
}
