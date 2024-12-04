#pragma once

#include <spargel/base/types.h>

namespace spargel::base {

    enum class OutputColor {
        Black,
        Red,
        Green,
        Yellow,
        Blue,
        Magenta,
        Cyan,
        White,

        Reset,
    };

    class OutputStream {
    public:
        ~OutputStream() {
            Flush();
            DestroyBuffer();
        }

        static OutputStream& Stdout();
        static OutputStream& Stderr();

        OutputStream& operator<<(char c) {
            if (IsFull()) Flush();
            *_cur++ = c;
            return *this;
        }
        OutputStream& operator<<(unsigned char c) {
            if (IsFull()) Flush();
            *_cur++ = c;
            return *this;
        }
        OutputStream& operator<<(signed char c) {
            if (IsFull()) Flush();
            *_cur++ = c;
            return *this;
        }
        OutputStream& operator<<(char const* s);
        OutputStream& operator<<(unsigned long long n);
        OutputStream& operator<<(long long n);
        OutputStream& operator<<(void const* p);

        OutputStream& operator<<(OutputColor c);

        void Flush();

        bool HasColor() const { return _color; }
        void EnableColor(bool enable) { _color = enable; }

    private:
        OutputStream(int fd) : _fd{fd} { CreateBuffer(); }

        void CreateBuffer();
        void DestroyBuffer();

        bool IsEmpty() const { return _cur == _begin; }
        bool IsFull() const { return _cur == _end; }

        void Write(char const* buf, ssize len);

        char* _begin = nullptr;
        char* _cur = nullptr;
        char* _end = nullptr;
        bool _color = true;
        int _fd = -1;

        static constexpr ssize BufferSize = 16 * 1024;
    };

}  // namespace spargel::base
