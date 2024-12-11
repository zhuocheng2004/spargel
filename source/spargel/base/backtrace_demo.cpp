#include <spargel/base/base.h>

static void foo() { spargel::base::print_backtrace(); }

int main() {
    foo();
    return 0;
}
