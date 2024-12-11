#pragma once

#include <spargel/base/meta.h>

namespace spargel::base {

    namespace __vector {

        template <typename T>
        class vector {
        public:
            vector() = default;
            ~vector() {}

        private:
            T* _begin = nullptr;
            T* _end = nullptr;
            T* _capacity = nullptr;
        };

    }  // namespace __vector

    using __vector::vector;

}  // namespace spargel::base
