#pragma once

namespace std {

using size_t = decltype(sizeof(0));

}  // namespace std

void* operator new(std::size_t, void*);
