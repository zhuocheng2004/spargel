#include <spargel/base/const.h>
#include <spargel/resource/directory.h>

// libc
#include <stdio.h>

namespace spargel::resource {

    void directory_resource::close() { fclose(_fp); }

    size_t directory_resource::size() {
        fseek(_fp, 0, SEEK_END);
        long size = ftell(_fp);
        return size;
    }

    void directory_resource::get_data(void* buf) {
        size_t s = size();
        fseek(_fp, 0, SEEK_SET);
        fread(buf, s, 1, _fp);
    }

    directory_resource* directory_resource_manager::open(const resource_id& id) {
        base::string real_path = _real_path(id);
        FILE* fp = fopen(real_path.data(), "rb");
        return fp ? new directory_resource(fp) : nullptr;
    }

    base::string directory_resource_manager::_real_path(const resource_id& id) {
        base::string root = _root_path.length() == 0 ? base::string(".") : _root_path;
        return root + PATH_SPLIT + id.path();
    }

}  // namespace spargel::resource
