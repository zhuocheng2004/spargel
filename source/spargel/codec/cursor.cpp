module;

#include <string.h>

#include <spargel/base/types.h>

module spargel.codec;

import :cursor;

bool scodec_cursor_is_end(struct scodec_cursor const* cursor) {
  return cursor->cur >= cursor->end;
}

u8 scodec_cursor_peek(struct scodec_cursor const* cursor) {
  if (scodec_cursor_is_end(cursor)) return 0;
  return *cursor->cur;
}

void scodec_cursor_advance(struct scodec_cursor* cursor) {
  if (!scodec_cursor_is_end(cursor)) cursor->cur++;
}

bool scodec_cursor_try_eat_char(struct scodec_cursor* cursor, char c) {
  if (cursor->cur + 1 > cursor->end) return false;
  if (scodec_cursor_peek(cursor) != c) return false;
  scodec_cursor_advance(cursor);
  return true;
}

bool scodec_cursor_try_eat_str(struct scodec_cursor* cursor, char const* str) {
  ssize len = (ssize)strlen(str);
  return scodec_cursor_try_eat_bytes(cursor, (u8 const*)str, len);
}

bool scodec_cursor_try_eat_bytes(struct scodec_cursor* cursor, u8 const* bytes,
                                 ssize len) {
  if (cursor->cur + len >= cursor->end) return false;
  if (memcmp(bytes, cursor->cur, len) != 0) return false;
  cursor->cur += len;
  return true;
}
