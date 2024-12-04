#include <spargel/base/output_stream.h>

#include <spargel/base/allocator.h>
#include <spargel/base/panic.h>

#include <string.h>

#if defined(SPARGEL_IS_MACOS) || defined(SPARGEL_IS_LINUX)
#include <unistd.h>
#endif

namespace spargel::base {

    namespace {
        void WriteToFd(int fd, char* buf, ssize len) {
#if defined(SPARGEL_IS_MACOS) || defined(SPARGEL_IS_LINUX)
            write(fd, buf, len);
#else
            Panic();
#endif
        }
    }  // namespace

    OutputStream& OutputStream::Stdout() {
        static OutputStream out(STDOUT_FILENO);
        return out;
    }

    OutputStream& OutputStream::Stderr() {
        static OutputStream out(STDERR_FILENO);
        return out;
    }

    void OutputStream::CreateBuffer() {
        _begin = static_cast<char*>(DefaultAllocator{}.allocate(BufferSize));
        _end = _begin + BufferSize;
        _cur = _begin;
    }

    void OutputStream::DestroyBuffer() {
        if (_begin != nullptr) {
            DefaultAllocator{}.deallocate(_begin, _end - _begin);
        }
    }

    void OutputStream::Flush() {
        if (IsEmpty()) return;
        WriteToFd(_fd, _begin, _cur - _begin);
        _cur = _begin;
    }

    OutputStream& OutputStream::operator<<(char const* s) {
        ssize len = static_cast<ssize>(strlen(s));
        Write(s, len);
        return *this;
    }

    void OutputStream::Write(char const* buf, ssize len) {
        if (_cur + len > _end) Flush();
        memcpy(_cur, buf, len);
        _cur += len;
    }

    OutputStream& OutputStream::operator<<(OutputColor c) {
        switch (c) {
        case OutputColor::Black:
            Write("\033[30m", 5);
            break;
        case OutputColor::Red:
            Write("\033[31m", 5);
            break;
        case OutputColor::Green:
            Write("\033[32m", 5);
            break;
        case OutputColor::Yellow:
            Write("\033[33m", 5);
            break;
        case OutputColor::Blue:
            Write("\033[34m", 5);
            break;
        case OutputColor::Magenta:
            Write("\033[35m", 5);
            break;
        case OutputColor::Cyan:
            Write("\033[36m", 5);
            break;
        case OutputColor::White:
            Write("\033[37m", 5);
            break;
        case OutputColor::Reset:
            Write("\033[39;49m", 8);
            break;
        default:
            break;
        }
        return *this;
    }

}  // namespace spargel::base
