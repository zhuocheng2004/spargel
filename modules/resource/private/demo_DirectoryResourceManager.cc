
#include <modules/resource/public/DirectoryResourceManager.h>
#include <stdio.h>
#include <stdlib.h>

static void printHex(unsigned char ch) {
  const char digits[] = "0123456789ABCDEFG";
  putchar(digits[ch >> 4]);
  putchar(digits[ch & 0xf]);
}

static void printRaw(unsigned char ch) {
  if (ch > ' ' && ch < '~')
    putchar(ch);
  else
    putchar('.');
}

int main(int argc, char* argv[]) {
  using namespace spargel::resource;

  if (argc <= 1) {
    fprintf(stderr, "Usage: %s <path> \n", argv[0]);
    return 1;
  }

  DirectoryResourceManager resourceManager(/*base*/ "");

  Resource* resource = resourceManager.open(ResourceID(argv[1]));
  if (!resource) {
    fprintf(stderr, "Cannot open \"%s\" \n", argv[1]);
    return 1;
  }

  size_t size = resource->size();
  printf("Size: %ld \n", size);

  void* buf = malloc(size);
  resource->getData(buf);
  resource->release();
  free(resource);

  printf("Content: \n");
  unsigned int line = 0, i;
  while (16 * line < size) {
    unsigned int c = size - 16 * line;
    c = c <= 16 ? c : 16;

    for (i = 0; i < c; i++) {
      printHex(((unsigned char*)buf)[16 * line + i]);
      putchar(' ');
    }
    for (; i < 16; i++) {
      putchar(' ');
      putchar(' ');
      putchar(' ');
    }

    putchar('|');
    putchar(' ');

    for (i = 0; i < c; i++) printRaw(((unsigned char*)buf)[16 * line + i]);

    putchar('\n');

    line++;
  }

  free(buf);

  resourceManager.close();

  return 0;
}
