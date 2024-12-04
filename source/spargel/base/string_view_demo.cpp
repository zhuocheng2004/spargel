#include <spargel/base/string_view.h>

#include <stdio.h>

void Foo(spargel::base::StringView view) { printf("length = %lld\n", view.Length()); }

int main() {
    Foo(spargel::base::StringLiteral{"foo"});
    return 0;
}
