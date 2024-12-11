#pragma once

#include <spargel/base/base.h>

namespace spargel::codec {

    struct cursor {
        char const* cur;
        char const* end;
    };

    bool cursor_is_end(struct cursor const* cursor);
    u8 cursor_peek(struct cursor const* cursor);
    void cursor_advance(struct cursor* cursor);
    bool cursor_try_eat_char(struct cursor* cursor, char c);
    bool cursor_try_eat_str(struct cursor* cursor, char const* str);
    bool cursor_try_eat_bytes(struct cursor* cursor, u8 const* bytes, ssize len);

}  // namespace spargel::codec
