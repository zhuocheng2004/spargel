
#include <spargel/base/base.h>
#include <spargel/base/logging.h>
#include <spargel/resource/resource.h>

namespace spargel::resource {

    void resource::close() {
        if (_mapped) {
            base::deallocate(_mapped, _mapped_size, base::ALLOCATION_RESOURCE);
        }
    }

    void* resource::map_data() {
        if (!_mapped) {
            _mapped_size = size();
            _mapped = base::allocate(_mapped_size, base::ALLOCATION_RESOURCE);
            get_data(_mapped);
        }
        return _mapped;
    }

}  // namespace spargel::resource
