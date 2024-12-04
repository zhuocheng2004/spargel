#include <spargel/base/source_location.h>

#include <stdio.h>

int main() {
    constexpr auto loc = spargel::base::SourceLocation::Current();
    printf("info: %s %s %u\n", loc.File(), loc.Function(), loc.Line());
    return 0;
}
