#pragma once

namespace spargel::base {

using nullptr_t = decltype(nullptr);

// signed size
using ssize = long long;
static_assert(sizeof(ssize) == 8);

using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;
static_assert(sizeof(u8) == 1);
static_assert(sizeof(u16) == 2);
static_assert(sizeof(u32) == 4);
static_assert(sizeof(u64) == 8);

}  // namespace spargel::base

using spargel::base::ssize;
using spargel::base::u16;
using spargel::base::u32;
using spargel::base::u64;
using spargel::base::u8;
