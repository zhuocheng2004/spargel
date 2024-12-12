#pragma once

#include <stddef.h>
#include <stdint.h>

// typedef long int ssize;
using ssize = ptrdiff_t;

// typedef unsigned char u8;
// typedef unsigned short int u16;
// typedef unsigned int u32;
// typedef unsigned long int u64;
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

// typedef signed char i8;
// typedef short int i16;
// typedef int i32;
// typedef long int i64;
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using nullptr_t = decltype(nullptr);
