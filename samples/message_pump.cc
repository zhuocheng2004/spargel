#include <stdio.h>

// CFRunLoop --> NSRunLoop --> NSApplication

struct CFRunLoop;

extern "C" {

CFRunLoop* CFRunLoopGetCurrent();

}

int main() {
  auto loop = CFRunLoopGetCurrent();
  if (loop) {
    printf("info: get non-null CFRunLoop\n");
  }
  return 0;
}
