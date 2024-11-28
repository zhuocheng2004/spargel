#pragma once

namespace spargel::base {

template <typename B, typename D>
concept is_base = __is_base_of(B, D);

}
