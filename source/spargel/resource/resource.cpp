
#include <spargel/base/platform.h>
#include <spargel/base/string.h>
#include <spargel/resource/directory.h>
#include <spargel/resource/resource.h>
#include <spargel/util/path.h>

namespace spargel::resource {

    resource_manager* default_resource_manager() {
        base::string root_path = spargel::util::dirname(spargel::base::get_executable_path()) +
                                 PATH_SPLIT + spargel::base::string_from_cstr("resources");

        return new directory_resource_manager(root_path);
    }

}  // namespace spargel::resource
