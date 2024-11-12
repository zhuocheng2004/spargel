#include "base/memory/ref_count_ptr.h"

#include "base/logging/logging.h"
#include "base/memory/ref_counted.h"

using base::memory::MakeRefCounted;
using base::memory::RefCounted;
using enum base::logging::LogLevel;

class Foo : public RefCounted<Foo> {
 public:
  Foo() { base::logging::Log(Debug, "Foo()"); }

 private:
  friend RefCounted<Foo>;
  ~Foo() { base::logging::Log(Debug, "~Foo()"); }
};

int main() {
  auto x = MakeRefCounted<Foo>();
  base::logging::Log(Debug, "x constructed");
  auto y = x;
  base::logging::Log(Debug, "before return");
  return 0;
}
