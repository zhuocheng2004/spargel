#pragma once

#include <spargel/base/types.h>
#include <spargel/resource/resource.h>

#if SPARGEL_FILE_MMAP

#if SPARGEL_IS_ANDROID || SPARGEL_IS_LINUX || SPARGEL_IS_MACOS
// POSIX
#include <sys/mman.h>
#endif

#else  // SPARGEL_FILE_MMAP

// libc
#include <stdio.h>

#endif  // SPARGEL_FILE_MMAP

namespace spargel::resource {

    class directory_resource : public resource {
        friend class directory_resource_manager;

    public:
        void close() override;

        usize size() override { return _size; }

        void get_data(void* buf) override;

        void* map_data() override;

    private:
        usize _size;
#if SPARGEL_FILE_MMAP
        int _fd;
        void* _mapped;

        directory_resource(usize size, int fd) : _size(size), _fd(fd), _mapped(nullptr) {}

        void* _map_data();
#else
        FILE* _fp;

        directory_resource(usize size, FILE* fp) : _size(size), _fp(fp) {}
#endif
    };

    class directory_resource_manager : public resource_manager {
    public:
        directory_resource_manager(base::string_view root_path) : _root_path(root_path) {}

        directory_resource* open(const resource_id& id) override;

    private:
        base::string _root_path;

        base::string _real_path(const resource_id& id);
    };

}  // namespace spargel::resource
