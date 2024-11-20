#include <assert.h>
#include <spargel/base/vector.h>

int main() {
  using spargel::base::Vector;
  Vector<int> vec;
  assert(vec.capacity() == 0);
  assert(vec.count() == 0);
  assert(vec.data() == nullptr);
  vec.pushCopy(1);
  assert(vec.capacity() > 0);
  assert(vec.count() == 1);
  assert(vec.data() != nullptr);
  vec.pushCopy(2);
  vec.pushCopy(3);
  assert(vec.count() == 3);
  for (int i = 4; i < 200; i++) {
    vec.pushCopy(i);
  }
  assert(vec.count() == 199);
  vec.pop();
  assert(vec.count() == 198);
  return 0;
}
