#pragma once

namespace std {

template <typename T>
using decay = __decay(T);

}
