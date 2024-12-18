
#include <spargel/base/const.h>
#include <spargel/base/logging.h>
#include <spargel/resource/directory.h>

// libc
#include <stdio.h>
#include <string.h>

#if SPARGEL_FILE_MMAP

#if SPARGEL_IS_ANDROID || SPARGEL_IS_LINUX || SPARGEL_IS_MACOS
// POSIX
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#endif

namespace spargel::resource {

    void directory_resource::close() {
#if SPARGEL_FILE_MMAP
        if (_mapped) {
            munmap(_mapped, _size);
        }
        ::close(_fd);
#else
        fclose(_fp);
#endif
        resource::close();
    }

    void directory_resource::get_data(void* buf) {
#if SPARGEL_FILE_MMAP
        if (!_mapped) {
            _mapped = _map_data();
        }
        memcpy(buf, _mapped, _size);
#else
        fseek(_fp, 0, SEEK_SET);
        fread(buf, _size, 1, _fp);
#endif
    }

    void* directory_resource::map_data() {
#if SPARGEL_FILE_MMAP
        if (!_mapped) {
            _mapped = _map_data();
        }
        return _mapped;
#else
        return resource::map_data();
#endif
    }

#if SPARGEL_FILE_MMAP
    void* directory_resource::_map_data() {
        return mmap(NULL, _size, PROT_READ, MAP_PRIVATE, _fd, 0);
    }
#endif

    directory_resource* directory_resource_manager::open(const resource_id& id) {
        base::string real_path = _real_path(id);
#if SPARGEL_FILE_MMAP
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
#else
        FILE* fp = fopen(real_path.data(), "rb");
        if (!fp) {
            return nullptr;
        }
        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        return new directory_resource(size, fp);
#endif
    }

    base::string directory_resource_manager::_real_path(const resource_id& id) {
        base::string root = _root_path.length() == 0 ? base::string(".") : _root_path;
        return root + PATH_SPLIT + id.path();
    }

}  // namespace spargel::resource
