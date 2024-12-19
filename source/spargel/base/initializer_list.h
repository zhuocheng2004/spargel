#pragma once

#include <spargel/base/types.h>

namespace std {

    /*
     * Caution: GCC requires this to have the same definition as in the standard <initializer_list>.
     */
    template <typename T>
    class initializer_list {
    public:
        constexpr initializer_list() noexcept = default;

        constexpr usize size() const { return _size; }
        constexpr T const* begin() const { return _begin; }
        constexpr T const* end() const { return _begin + _size; }

    private:
        T const* _begin = nullptr;
        usize _size;
    };

}  // namespace std

namespace spargel::base {

    using std::initializer_list;

}
