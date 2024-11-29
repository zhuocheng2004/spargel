#pragma once

namespace spargel::base {

template <typename T>
using decay = __decay(T);

}
