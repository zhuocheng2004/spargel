#pragma once

namespace std {

template <typename From, typename To>
concept is_conertible = __is_convertible(From, To);

}