#pragma once

namespace spargel::base {

template <typename T>
using remove_reference = __remove_reference_t(T);

}
