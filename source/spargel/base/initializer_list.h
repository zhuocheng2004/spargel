#pragma once

#include <spargel/base/types.h>

namespace std {

    template <typename T>
    class initializer_list {
    public:
        constexpr initializer_list() noexcept = default;
        
        constexpr T const* begin() const { return _begin; }
        constexpr T const* end() const { return _end; }

    private:
        T const* _begin = nullptr;
        T const* _end = nullptr;
    };

}

namespace spargel::base {

    using std::initializer_list;

}
