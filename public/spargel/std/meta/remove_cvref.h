#pragma once

namespace std {

template <typename T>
using remove_cvref = __remove_cvref(T);

}
