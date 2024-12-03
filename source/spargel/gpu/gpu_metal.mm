#import <Metal/Metal.h>

@import spargel.base.pointer;
import spargel.gpu;

namespace spargel::gpu {

    class MetalDevice final : public Device {
    public:
        ~MetalDevice() override = default;

    private:
        id<MTLDevice> _device = nil;
    };

    base::UniquePtr<Device> CreateMetalDevice() {
        return base::MakeUnique<MetalDevice>();
    }
}
