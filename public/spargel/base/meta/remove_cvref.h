#pragma once

namespace spargel::base {

template <typename T>
using remove_cvref = __remove_cvref(T);

}
