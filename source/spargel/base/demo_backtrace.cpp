import spargel.base.c;

static void foo() { sbase_print_backtrace(); }

int main() {
  foo();
  return 0;
}
