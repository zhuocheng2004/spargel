#include <spargel/base/get_executable_path.h>
#include <stdio.h>
#include <windows.h>

int main() {
  using namespace spargel::base;

  char buf[PATH_MAX + 1];
  int len = getExecutablePath(buf, PATH_MAX);
  buf[len] = '\0';
  printf("Current executable path: \"%s\" \n", buf);

  return 0;
}
