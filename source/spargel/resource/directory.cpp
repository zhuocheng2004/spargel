
#include <spargel/base/const.h>
#include <spargel/resource/directory.h>

// libc
#include <string.h>

#if SPARGEL_FILE_MMAP

#if SPARGEL_IS_ANDROID || SPARGEL_IS_LINUX || SPARGEL_IS_MACOS
// POSIX
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#else  // SPARGEL_FILE_MMAP

// libc
#include <stdio.h>

#endif  // SPARGEL_FILE_MMAP

namespace spargel::resource {

#if SPARGEL_FILE_MMAP

    void directory_resource::close() {
        if (_mapped) {
            munmap(_mapped, _size);
        }
        ::close(_fd);
        resource::close();
    }

    void directory_resource::get_data(void* buf) {
        if (!_mapped) {
            _mapped = _map_data();
        }
        memcpy(buf, _mapped, _size);
    }

    void* directory_resource::map_data() {
        if (!_mapped) {
            _mapped = _map_data();
        }
        return _mapped;
    }

    void* directory_resource::_map_data() {
        return mmap(NULL, _size, PROT_READ, MAP_PRIVATE, _fd, 0);
    }

    directory_resource* directory_resource_manager::open(const resource_id& id) {
        base::string real_path = _real_path(id);
        int fd = ::open(real_path.data(), O_RDONLY);
        if (fd < 0) {
            return nullptr;
        }
        struct stat sb;
        if (fstat(fd, &sb) < 0) {
            ::close(fd);
            return nullptr;
        }

        return new directory_resource(sb.st_size, fd);
    }

#else  // SPARGEL_FILE_MMAP

    void directory_resource::close() {
        fclose(_fp);
        resource::close();
    }

    void directory_resource::get_data(void* buf) {
        fseek(_fp, 0, SEEK_SET);
        fread(buf, _size, 1, _fp);
    }

    void* directory_resource::map_data() { return resource::map_data(); }

    directory_resource* directory_resource_manager::open(const resource_id& id) {
        base::string real_path = _real_path(id);
        FILE* fp = fopen(real_path.data(), "rb");
        if (!fp) {
            return nullptr;
        }
        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        return new directory_resource(size, fp);
    }
#endif

    base::string directory_resource_manager::_real_path(const resource_id& id) {
        base::string root = _root_path.length() == 0 ? base::string(".") : _root_path;
        return root + PATH_SPLIT + id.path();
    }

}  // namespace spargel::resource
