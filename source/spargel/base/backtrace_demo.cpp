#include <spargel/base/base.h>

void foo() { spargel::base::print_backtrace(); }

int main() {
    foo();
    return 0;
}
