#include <spargel/base/base.h>
#include <stdio.h>

/* stupid value */
#define PATH_MAX 1024

int main() {
  char buf[PATH_MAX + 1];
  ssize len = spargel_get_executable_path(buf, PATH_MAX);
  buf[len] = '\0';
  printf("Current executable path: \"%s\" \n", buf);
  printf("Path length: %td\n", len);
  return 0;
}
