#pragma once

namespace std {

template <typename L, typename R>
concept is_same = __is_same(L, R);

}
