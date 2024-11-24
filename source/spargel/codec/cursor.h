#pragma once

#include <spargel/base/base.h>

struct scodec_cursor {
  char const* cur;
  char const* end;
};

bool scodec_cursor_is_end(struct scodec_cursor const* cursor);
u8 scodec_cursor_peek(struct scodec_cursor const* cursor);
void scodec_cursor_advance(struct scodec_cursor* cursor);
bool scodec_cursor_try_eat_char(struct scodec_cursor* cursor, char c);
bool scodec_cursor_try_eat_str(struct scodec_cursor* cursor, char const* str);
bool scodec_cursor_try_eat_bytes(struct scodec_cursor* cursor, u8 const* bytes, ssize len);
