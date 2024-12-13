#include <spargel/config.h>
#include <spargel/gpu/gpu3.h>

#if SPARGEL_GPU_ENABLE_METAL
namespace spargel::gpu {
    base::unique_ptr<device> make_device_metal();
}
#endif

namespace spargel::gpu {

    base::unique_ptr<device> make_device(device_kind kind) {
        switch (kind) {
#if SPARGEL_GPU_ENABLE_METAL
        case device_kind::metal:
            return make_device_metal();
#endif
        default:
            return nullptr;
        }
    }

}  // namespace spargel::gpu
