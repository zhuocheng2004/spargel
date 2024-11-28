#pragma once

namespace std {

template <typename T>
concept is_trivially_relocatable = __is_trivially_relocatable(T);

}
