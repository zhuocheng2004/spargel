#include "base/memory/ref_count_ptr.h"

#include "base/logging/logging.h"
#include "base/memory/ref_counted.h"

using base::memory::MakeRefCounted;
using base::memory::RefCounted;

class Foo : public RefCounted<Foo> {
 public:
  Foo() { base::logging::Log("Foo()"); }

 private:
  friend RefCounted<Foo>;
  ~Foo() { base::logging::Log("~Foo()"); }
};

int main() {
  auto x = MakeRefCounted<Foo>();
  base::logging::Log("x constructed");
  auto y = x;
  base::logging::Log("before return");
  return 0;
}
