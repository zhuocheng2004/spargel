#include <spargel/base/backtrace.h>

static void foo() { spargel::base::PrintBacktrace(); }

int main() {
  foo();
  return 0;
}
