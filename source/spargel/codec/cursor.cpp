#include <spargel/codec/cursor.h>

/* libc */
#include <string.h>

namespace spargel::codec {

    bool cursor_is_end(struct cursor const* cursor) { return cursor->cur >= cursor->end; }

    u8 cursor_peek(struct cursor const* cursor) {
        if (cursor_is_end(cursor)) return 0;
        return *cursor->cur;
    }

    void cursor_advance(struct cursor* cursor) {
        if (!cursor_is_end(cursor)) cursor->cur++;
    }

    bool cursor_try_eat_char(struct cursor* cursor, char c) {
        if (cursor->cur + 1 > cursor->end) return false;
        if (cursor_peek(cursor) != c) return false;
        cursor_advance(cursor);
        return true;
    }

    bool cursor_try_eat_str(struct cursor* cursor, char const* str) {
        ssize len = (ssize)strlen(str);
        return cursor_try_eat_bytes(cursor, (u8 const*)str, len);
    }

    bool cursor_try_eat_bytes(struct cursor* cursor, u8 const* bytes, ssize len) {
        if (cursor->cur + len >= cursor->end) return false;
        if (memcmp(bytes, cursor->cur, len) != 0) return false;
        cursor->cur += len;
        return true;
    }

}  // namespace spargel::codec
