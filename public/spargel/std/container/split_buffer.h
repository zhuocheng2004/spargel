#pragma once

namespace std {

template <typename T>
class split_buffer {
public:
    split_buffer() = default;

private:
    T* _first = nullptr;
    T* _last = nullptr;
    T* _begin = nullptr;
    T* _end = nullptr;
};

}  // namespace spargel::base
