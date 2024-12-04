#include <spargel/base/string_literal.h>
#include <spargel/base/output_stream.h>

int main() {
    constexpr spargel::base::StringLiteral str = "hello";
    static_assert(str.Length() == 5);
    spargel::base::OutputStream::Stdout() << str.Data() << '\n';
    return 0;
}
