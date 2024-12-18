#pragma once

#include <spargel/base/types.h>
#include <spargel/base/unique_ptr.h>
#include <spargel/resource/resource.h>

#if !SPARGEL_FILE_MMAP

// libc
#include <stdio.h>

#endif  // !SPARGEL_FILE_MMAP

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
        void* _mapped;

#if SPARGEL_IS_POSIX
        int _fd;
        directory_resource(usize size, int fd) : _size(size), _mapped(nullptr), _fd(fd) {}
#else
#error unimplemented
#endif

        void* _map();
        void _unmap(void* ptr, usize size);
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

    base::unique_ptr<directory_resource_manager> make_relative_manager();

}  // namespace spargel::resource
