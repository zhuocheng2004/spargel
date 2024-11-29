#pragma once

namespace spargel::base {

template <typename T>
concept is_trivially_relocatable = __is_trivially_relocatable(T);

}
